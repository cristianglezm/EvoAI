#include "Simulation.hpp"
#include "Render.hpp"
#include "TaskQueue.hpp"
#include "Ansi.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>
#include <EvoAI.hpp>
#include <EvoAI/Utils.hpp>

namespace PFA = EvoAI::PathFindingAlgorithms;

Metrics runSimulation(const GraphT& graph,
                      std::vector<std::unique_ptr<LetterState>>& letters,
                      const std::vector<char>& overlay)
{
    using PolicyT = PFA::DefaultTraversalPolicy<GraphT>;

    EvoAI::Guard<Metrics> guardedMetrics;
    TaskQueue taskQueue;
    std::atomic<int> tasksRemaining{static_cast<int>(letters.size())};

    for(std::size_t i = 0; i < letters.size(); ++i){
		taskQueue.push(Task{i});
	}
    auto workerFn = [&](int workerId){
        // One SearchContext/policy per worker thread, reused across every letter that
        // worker pulls from the queue.
        PFA::SearchContext ctx;
        PolicyT policy;
        Task task;
        while(taskQueue.pop(task)){
            std::size_t li = task.letterIndex;
            if(li >= letters.size()){
				tasksRemaining.fetch_sub(1, std::memory_order_relaxed); 
				continue;
			}
            LetterState& ls = *letters[li];

            std::vector<const GraphT::Node_t*> path;
            const auto& startNode = graph[ls.currentIdx.load()];
            const auto& goalNode  = graph[ls.goalIdx];

            auto t0 = std::chrono::high_resolution_clock::now();
            switch(ls.algo){
                case Algorithm::DFS: {
					PFA::DFS<GraphT,PolicyT> a; 
					path = a(graph, startNode, goalNode, ctx, policy); 
					break;
				}
                case Algorithm::BFS: {
					PFA::BFS<GraphT,PolicyT> a; 
					path = a(graph, startNode, goalNode, ctx, policy); 
					break;
				}
                case Algorithm::GBFS: { 
					PFA::GBFS<GraphT,PolicyT> a; 
					path = a(graph, startNode, goalNode, ctx, policy); 
					break;
				}
                case Algorithm::ASTAR: {
					PFA::AStar<GraphT,PolicyT> a; 
					path = a(graph, startNode, goalNode, ctx, policy); 
					break;
				}
            }
            auto t1 = std::chrono::high_resolution_clock::now();
            long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

            guardedMetrics.visit([&](Metrics& m){ m.record(ns, path.size()); });
            {
                std::lock_guard<std::mutex> lk(drawMutex);
                std::cerr << "[" << workerId << "] '" << ls.ch << "' "
                          << algoColor(ls.algo) << algoName(ls.algo) << RESET
                          << " len=" << path.size() << " ns=" << ns << "\n";
            }

            if(!path.empty()){
                constexpr std::size_t maxFrames = 40;
                const std::size_t stride = std::max(std::size_t{1}, path.size() / maxFrames);
                for(std::size_t step = 0; step < path.size(); step += stride){
                    ls.currentIdx.store(path[step]->index, std::memory_order_relaxed);
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
            }
            ls.currentIdx.store(ls.goalIdx, std::memory_order_relaxed);
            ls.finished.store(true, std::memory_order_relaxed);
            tasksRemaining.fetch_sub(1, std::memory_order_relaxed);
        }
    };

    const unsigned hw = std::max(1u, std::thread::hardware_concurrency());
    const unsigned poolSize = static_cast<unsigned>(std::min<std::size_t>(letters.size(), hw));
    std::vector<std::thread> workers;
    workers.reserve(poolSize);
    for(unsigned i = 0; i < poolSize; ++i){
		workers.emplace_back(workerFn, static_cast<int>(i));
	}
    constexpr int animDelay = 5;
    while(tasksRemaining.load(std::memory_order_relaxed) > 0){
        render(graph, overlay, letters);
        std::this_thread::sleep_for(std::chrono::milliseconds(animDelay));
    }
    taskQueue.shutdown();
    for(auto& w : workers){
		if(w.joinable()){
			w.join();
		}
	}
    return guardedMetrics.visit([](Metrics& m){ return std::move(m); });
}

void printMetrics(const Metrics& m, 
	const std::vector<std::unique_ptr<LetterState>>& letters, 
	const SimConfig& cfg){

    long long   totalNs = 0, minNs = std::numeric_limits<long long>::max(), maxNs = 0;
    std::size_t totalLen = 0;
    for(auto v : m.timesNs){
		totalNs += v; 
		minNs = std::min(minNs,v); 
		maxNs = std::max(maxNs,v);
	}
    for(auto l : m.pathLengths){
		totalLen += l;
	}
    if(m.timesNs.empty()){
		minNs = 0;
	}
    const std::size_t count  = m.timesNs.size();
    const double avgNs = count ? static_cast<double>(totalNs) / count : 0.0;
    const double avgLen = count ? static_cast<double>(totalLen) / count : 0.0;
    std::cout << "\n" << BOLD << "-- Config -----------------------------------" << RESET << "\n"
              << "  seed        : " << cfg.seed       << "\n"
              << "  randomness  : " << cfg.randomness << "\n"
              << "  binomial    : " << cfg.binomial   << "\n"
              << "  grid        : " << cfg.cols << " x " << cfg.rows << "\n"
              << "\n" << BOLD << "-- Pathfinding metrics ----------------------" << RESET << "\n"
              << "  tasks       : " << count                              << "\n"
              << "  avg time    : " << static_cast<long long>(avgNs)  << " ns\n"
              << "  min time    : " << minNs                              << " ns\n"
              << "  max time    : " << maxNs                              << " ns\n"
              << "  avg path len: " << avgLen                             << "\n"
              << "\n" << BOLD << "-- Per-algorithm breakdown ------------------" << RESET << "\n";
    for(int a = 0; a < 4; ++a){
        Algorithm algo = static_cast<Algorithm>(a);
        long long sum = 0; std::size_t cnt = 0;
        for(std::size_t i = 0; i < letters.size() && i < m.timesNs.size(); ++i){
            if(letters[i]->algo == algo){
				sum += m.timesNs[i];
				++cnt;
			}
		}
        if(cnt > 0){
            std::cout << "  " << algoColor(algo) << algoName(algo) << RESET
                      << "\t avg " << sum / static_cast<long long>(cnt) << " ns"
                      << "  (" << cnt << " letter" << (cnt > 1 ? "s" : "") << ")\n";
		}
    }
}
