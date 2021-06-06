#include "crpc_server.h"
#include <gtest/gtest.h> 

TEST(Add, 负数) {  
 EXPECT_EQ(Add(-1,-2), -3);  
 EXPECT_GT(Add(-4,-5), -6); // 故意的  
}

TEST(Add, 正数) {  
 EXPECT_EQ(Add(1,2), 3);  
 EXPECT_GT(Add(4,5), 6);  
} 