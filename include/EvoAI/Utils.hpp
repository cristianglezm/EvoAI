#ifndef EVOAI_UTILS_HPP
#define EVOAI_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <thread>
#include <mutex>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

#include <EvoAI/Utils/MathUtils.hpp>
#include <EvoAI/Utils/NNUtils.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

namespace EvoAI{
    /**
     *  @brief calculates time it takes Fn in milliseconds
     *  
     *  @tparam Fn funtion to evaluate
     *  @tparam Args args for funtion
     *  @param fn   function to measure
     *  @param args args for function
     *  @return double
     */
    template<typename Fn, typename...Args>
    auto TimeThis(Fn&& fn, const Args&...args) noexcept{
        auto start = std::chrono::high_resolution_clock::now();
        std::invoke(fn, args...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end-start).count();
    }
    /**
     *  @brief calculates time it takes Fn in milliseconds
     *
     *  @tparam Fn funtion to evaluate
     *  @param fn function to measure
     *  @return double milliseconds
     */
    template<typename Fn>
    auto TimeThis(Fn&& fn) noexcept{
        auto start = std::chrono::high_resolution_clock::now();
        fn();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end-start).count();
    }
    namespace priv{
        template<typename NamesTuple, typename JsonsTuple, std::size_t...Is>
        JsonBox::Value makeJsonFrom_impl(const NamesTuple& names, const JsonsTuple& jsons, std::index_sequence<Is...>) noexcept{
            JsonBox::Object o;
            ((o[std::get<Is>(names)] = std::get<Is>(jsons).toJson()), ...);
            return o;
        }
        template<typename...Jsons, std::size_t...Is>
        std::tuple<Jsons...> loadJsonFrom_impl(const std::string& filename, const std::array<std::string,sizeof...(Jsons)>& names, std::index_sequence<Is...>) noexcept{
            JsonBox::Value v;
            v.loadFromFile(filename);
            return std::make_tuple<Jsons...>(Jsons(v[std::get<Is>(names)].getObject())...);
        }
    }
    /**
     * @brief creates a json object from each Jsons::toJson() to the given names.
     * @warning Jsons should have implemented ::toJson() const noexcept;
     * @code
     *      // class to serialize
     *      auto ffnn = EvoAI::createFeedForwardNN(2,3,{1,5,3},2, 1.0);
     *      // convert ffnn to json into key "ffnn" and EvoAI::Scaler<double> to "sc"
     *      auto v = EvoAI::makeJsonFrom({"ffnn","sc"},*ffnn,EvoAI::Scaler<double>());
     *      // write to file
     *      v.writeToFile("ffnnAndSCTest.json");
     * @endcode
     * @tparam Jsons...
     * @param names std::array<std::String,sizeof...(Jsons)>
     * @param jsons objects to call ::toJson()
     * @return JsonBox::Value
     */
    template<typename...Jsons>
    JsonBox::Value makeJsonFrom(const std::array<std::string, sizeof...(Jsons)>& names, const Jsons&...jsons) noexcept{
        static_assert((meta::serializable_v<Jsons> &&...), "The Jsons need to be serializable_v, more info at EvoAI/Utils/TypeUtils.hpp");
        const auto jsonsTuple = std::make_tuple<const Jsons&...>(jsons...);
        return priv::makeJsonFrom_impl(names, jsonsTuple, std::index_sequence_for<Jsons...>{});
    }
    /**
     * @brief returns a std::tuple with the deserialized classes from the json.
     * @warning Jsons should have implemented a Jsons::Jsons(JsonBox::Object) constructor.
     * @code
     *      // read from a json filename and deserialize into ffNN and sc
     *      auto [ffNN, sc] = EvoAI::loadJsonFrom<NeuralNetwork,Scaler<double>>("ffnnAndSCTest.json", {"ffnn", "sc"});
     * @endcode
     * @param filename std::string
     * @param names std::array<std::string,sizeof...(Jsons)>
     * @return std::tuple<Jsons...>
     */
    template<typename...Jsons>
    std::tuple<Jsons...> loadJsonFrom(const std::string& filename, const std::array<std::string, sizeof...(Jsons)>& names) noexcept{
        static_assert((meta::serializable_constructible_v<Jsons> &&...), "The Jsons need to be serializable_constructible_v, more info at EvoAI/Utils/TypeUtils.hpp");
        return priv::loadJsonFrom_impl<Jsons...>(filename, names, std::index_sequence_for<Jsons...>{});
    }
    /**
     * @brief simple csv file parser it will read the file and returns a std::vector<std::vector<std::string>>
     * @param istr std::istream&
     * @param delimiter char& defaulted to comma
     * @return std::vector<std::vector<std::string>> with the csv data.
     */
    EvoAI_API std::vector<std::vector<std::string>> readCSVFile(std::istream& istr, char delimiter = ',');
    /**
     * @brief writes a csv file.
     * @param csvData const std::vector<std::vector<std::string>>&
     * @param ostr std::ostream&
     * @param delimiter char
     */
    EvoAI_API void writeCSVFile(const std::vector<std::vector<std::string>>& csvData, std::ostream& ostr, char delimiter = ',');
    /**
     * @brief guards a T from multiple threads accessing it.
     * @tparam T object to guard
     */
    template<class T>
    class EvoAI_API Guard{
        public:
            /**
             * @brief constructor without args
             */
            Guard() noexcept
            : mtx()
            , value(){}
            /**
             * @brief constructor with args
             * @tparam ...Args pack
             * @param ...args args to construct T
             */
            template<typename...Args>
            Guard(Args&&...args) noexcept
            : mtx()
            , value((std::forward<Args>(args), ...)){}
            /**
             * @brief visit the protected data
             * @tparam Fn function
             * @param fn Fn&& 
             * @return auto
             */
            template<typename Fn>
            auto visit(Fn&& fn) noexcept{
                std::lock_guard lg(mtx);
                return fn(value);
            }
        private:
            std::mutex mtx;
            T value;
    };
}

#endif // EVOAI_UTILS_HPP