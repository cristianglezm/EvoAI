#ifndef EVOAI_CONNECTION_TEST_HPP
#define EVOAI_CONNECTION_TEST_HPP

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        class ConnectionTest : public ::testing::Test {
            protected:
                // If the constructor and destructor are not enough for setting up
                // and cleaning up each test, you can define the following methods:
                // Code here will be called immediately after the constructor (right
                // before each test).
                ConnectionTest();
                virtual void SetUp(){

                }
                // Code here will be called immediately after each test (right
                // before the destructor).
                virtual void TearDown(){
                    
                }
                // You can do clean-up work that doesn't throw exceptions here.
                virtual ~ConnectionTest() = default;
        public:
            Connection c1;
            Connection c2;
        };
        ConnectionTest::ConnectionTest():c1(Link(0,1), Link(1,0),-0.2344), c2(Link(1,1), Link(0,1)){}
        TEST_F(ConnectionTest,ConstructorWithWeight){
            EXPECT_EQ(-0.2344,c1.getWeight());
            EXPECT_EQ(0, c1.getCycles());
            EXPECT_FALSE(c1.isRecurrent());
        }
        TEST_F(ConnectionTest, ConstructorWithLinks){
            EXPECT_EQ(0.0,c2.getWeight());
            EXPECT_EQ(0, c2.getCycles());
            EXPECT_TRUE(c2.isRecurrent());
        }
    }
}
#endif // EVOAI_CONNECTION_TEST_HPP