// SPDX-License-Identifier: CDDL-1.0
/*
 * Comprehensive unit tests for FreeBSD AVL tree implementation
 * Using Google Test framework
 */

#include <gtest/gtest.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>

extern "C" {
#include "avl.h"
#include "avl_impl.h"
}

// Test data structure
struct TestNode {
    int key;
    char name[32];
    avl_node_t avl_link;

    TestNode(int k = 0, const char* n = "") : key(k) {
        std::strncpy(name, n, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        std::memset(&avl_link, 0, sizeof(avl_link));
    }
};

// Comparator function for TestNode
static int test_node_compare(const void *a, const void *b) {
    const TestNode *na = static_cast<const TestNode*>(a);
    const TestNode *nb = static_cast<const TestNode*>(b);
    return TREE_CMP(na->key, nb->key);
}

// Test fixture for AVL tree tests
class AVLTreeTest : public ::testing::Test {
protected:
    avl_tree_t tree;

    void SetUp() override {
        avl_create(&tree, test_node_compare, sizeof(TestNode),
                   offsetof(TestNode, avl_link));
    }

    void TearDown() override {
        void *cookie = nullptr;
        TestNode *node;
        while ((node = static_cast<TestNode*>(avl_destroy_nodes(&tree, &cookie))) != nullptr) {
            // Node cleanup if needed
        }
        avl_destroy(&tree);
    }

    // Helper function to verify tree invariants
    void VerifyTreeInvariants() {
        if (avl_is_empty(&tree)) {
            EXPECT_EQ(avl_numnodes(&tree), 0);
            EXPECT_EQ(avl_first(&tree), nullptr);
            EXPECT_EQ(avl_last(&tree), nullptr);
            return;
        }

        EXPECT_GT(avl_numnodes(&tree), 0);
        EXPECT_NE(avl_first(&tree), nullptr);
        EXPECT_NE(avl_last(&tree), nullptr);

        // Verify in-order traversal produces sorted sequence
        TestNode *prev = nullptr;
        TestNode *curr = static_cast<TestNode*>(avl_first(&tree));
        ulong_t count = 0;

        while (curr != nullptr) {
            count++;
            if (prev != nullptr) {
                EXPECT_LT(prev->key, curr->key);
            }
            prev = curr;
            curr = static_cast<TestNode*>(AVL_NEXT(&tree, curr));
        }

        EXPECT_EQ(count, avl_numnodes(&tree));
    }
};

// Test: Tree creation and initialization
TEST_F(AVLTreeTest, CreateEmpty) {
    EXPECT_EQ(avl_numnodes(&tree), 0);
    EXPECT_TRUE(avl_is_empty(&tree));
    EXPECT_EQ(avl_first(&tree), nullptr);
    EXPECT_EQ(avl_last(&tree), nullptr);
}

// Test: Single node insertion
TEST_F(AVLTreeTest, InsertSingleNode) {
    TestNode node1(10, "Node10");
    avl_add(&tree, &node1);

    EXPECT_EQ(avl_numnodes(&tree), 1);
    EXPECT_FALSE(avl_is_empty(&tree));
    EXPECT_EQ(avl_first(&tree), &node1);
    EXPECT_EQ(avl_last(&tree), &node1);

    VerifyTreeInvariants();
}

// Test: Multiple node insertion
TEST_F(AVLTreeTest, InsertMultipleNodes) {
    TestNode nodes[] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(40, "Node40"),
        TestNode(60, "Node60"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    EXPECT_EQ(avl_numnodes(&tree), 7);
    EXPECT_FALSE(avl_is_empty(&tree));

    VerifyTreeInvariants();
}

// Test: Insertion with balancing (left-left case)
TEST_F(AVLTreeTest, InsertLeftLeftCase) {
    TestNode node3(30, "Node30");
    TestNode node2(20, "Node20");
    TestNode node1(10, "Node10");

    avl_add(&tree, &node3);
    avl_add(&tree, &node2);
    avl_add(&tree, &node1); // Should trigger left-left rotation

    EXPECT_EQ(avl_numnodes(&tree), 3);
    VerifyTreeInvariants();
}

// Test: Insertion with balancing (right-right case)
TEST_F(AVLTreeTest, InsertRightRightCase) {
    TestNode node1(10, "Node10");
    TestNode node2(20, "Node20");
    TestNode node3(30, "Node30");

    avl_add(&tree, &node1);
    avl_add(&tree, &node2);
    avl_add(&tree, &node3); // Should trigger right-right rotation

    EXPECT_EQ(avl_numnodes(&tree), 3);
    VerifyTreeInvariants();
}

// Test: Insertion with balancing (left-right case)
TEST_F(AVLTreeTest, InsertLeftRightCase) {
    TestNode node3(30, "Node30");
    TestNode node1(10, "Node10");
    TestNode node2(20, "Node20");

    avl_add(&tree, &node3);
    avl_add(&tree, &node1);
    avl_add(&tree, &node2); // Should trigger left-right rotation

    EXPECT_EQ(avl_numnodes(&tree), 3);
    VerifyTreeInvariants();
}

// Test: Insertion with balancing (right-left case)
TEST_F(AVLTreeTest, InsertRightLeftCase) {
    TestNode node1(10, "Node10");
    TestNode node3(30, "Node30");
    TestNode node2(20, "Node20");

    avl_add(&tree, &node1);
    avl_add(&tree, &node3);
    avl_add(&tree, &node2); // Should trigger right-left rotation

    EXPECT_EQ(avl_numnodes(&tree), 3);
    VerifyTreeInvariants();
}

// Test: Find existing node
TEST_F(AVLTreeTest, FindExistingNode) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    TestNode search_key(30, "");
    avl_index_t where;
    TestNode *found = static_cast<TestNode*>(avl_find(&tree, &search_key, &where));

    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->key, 30);
    EXPECT_STREQ(found->name, "Node30");
}

// Test: Find non-existing node
TEST_F(AVLTreeTest, FindNonExistingNode) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    TestNode search_key(35, "");
    avl_index_t where;
    TestNode *found = static_cast<TestNode*>(avl_find(&tree, &search_key, &where));

    EXPECT_EQ(found, nullptr);
}

// Test: avl_insert with where parameter
TEST_F(AVLTreeTest, InsertWithWhere) {
    TestNode node1(50, "Node50");
    TestNode node2(30, "Node30");

    avl_add(&tree, &node1);

    avl_index_t where;
    TestNode *found = static_cast<TestNode*>(avl_find(&tree, &node2, &where));
    EXPECT_EQ(found, nullptr);

    avl_insert(&tree, &node2, where);
    EXPECT_EQ(avl_numnodes(&tree), 2);

    VerifyTreeInvariants();
}

// Test: avl_insert_here with AVL_BEFORE
TEST_F(AVLTreeTest, InsertHereBefore) {
    TestNode node1(50, "Node50");
    TestNode node2(30, "Node30");

    avl_add(&tree, &node1);
    avl_insert_here(&tree, &node2, &node1, AVL_BEFORE);

    EXPECT_EQ(avl_numnodes(&tree), 2);
    EXPECT_EQ(avl_first(&tree), &node2);

    VerifyTreeInvariants();
}

// Test: avl_insert_here with AVL_AFTER
TEST_F(AVLTreeTest, InsertHereAfter) {
    TestNode node1(50, "Node50");
    TestNode node2(70, "Node70");

    avl_add(&tree, &node1);
    avl_insert_here(&tree, &node2, &node1, AVL_AFTER);

    EXPECT_EQ(avl_numnodes(&tree), 2);
    EXPECT_EQ(avl_last(&tree), &node2);

    VerifyTreeInvariants();
}

// Test: Remove single node
TEST_F(AVLTreeTest, RemoveSingleNode) {
    TestNode node1(50, "Node50");
    avl_add(&tree, &node1);

    avl_remove(&tree, &node1);

    EXPECT_EQ(avl_numnodes(&tree), 0);
    EXPECT_TRUE(avl_is_empty(&tree));
}

// Test: Remove leaf node
TEST_F(AVLTreeTest, RemoveLeafNode) {
    TestNode nodes[3] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    avl_remove(&tree, &nodes[1]); // Remove leaf node 30

    EXPECT_EQ(avl_numnodes(&tree), 2);
    VerifyTreeInvariants();
}

// Test: Remove node with one child
TEST_F(AVLTreeTest, RemoveNodeWithOneChild) {
    TestNode nodes[4] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    avl_remove(&tree, &nodes[1]); // Remove node 30 which has one child (20)

    EXPECT_EQ(avl_numnodes(&tree), 3);
    VerifyTreeInvariants();
}

// Test: Remove node with two children
TEST_F(AVLTreeTest, RemoveNodeWithTwoChildren) {
    TestNode nodes[7] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(40, "Node40"),
        TestNode(60, "Node60"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    avl_remove(&tree, &nodes[0]); // Remove root node 50 which has two children

    EXPECT_EQ(avl_numnodes(&tree), 6);
    VerifyTreeInvariants();
}

// Test: AVL_NEXT traversal
TEST_F(AVLTreeTest, NextTraversal) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    std::vector<int> keys;
    TestNode *curr = static_cast<TestNode*>(avl_first(&tree));
    while (curr != nullptr) {
        keys.push_back(curr->key);
        curr = static_cast<TestNode*>(AVL_NEXT(&tree, curr));
    }

    std::vector<int> expected = {20, 30, 50, 70, 80};
    EXPECT_EQ(keys, expected);
}

// Test: AVL_PREV traversal
TEST_F(AVLTreeTest, PrevTraversal) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    std::vector<int> keys;
    TestNode *curr = static_cast<TestNode*>(avl_last(&tree));
    while (curr != nullptr) {
        keys.push_back(curr->key);
        curr = static_cast<TestNode*>(AVL_PREV(&tree, curr));
    }

    std::vector<int> expected = {80, 70, 50, 30, 20};
    EXPECT_EQ(keys, expected);
}

// Test: avl_first and avl_last
TEST_F(AVLTreeTest, FirstAndLast) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    TestNode *first = static_cast<TestNode*>(avl_first(&tree));
    TestNode *last = static_cast<TestNode*>(avl_last(&tree));

    ASSERT_NE(first, nullptr);
    ASSERT_NE(last, nullptr);
    EXPECT_EQ(first->key, 20);
    EXPECT_EQ(last->key, 80);
}

// Test: avl_nearest with AVL_BEFORE
TEST_F(AVLTreeTest, NearestBefore) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    TestNode search_key(35, "");
    avl_index_t where;
    TestNode *found = static_cast<TestNode*>(avl_find(&tree, &search_key, &where));
    EXPECT_EQ(found, nullptr);

    TestNode *nearest = static_cast<TestNode*>(avl_nearest(&tree, where, AVL_BEFORE));
    ASSERT_NE(nearest, nullptr);
    EXPECT_EQ(nearest->key, 30);
}

// Test: avl_nearest with AVL_AFTER
TEST_F(AVLTreeTest, NearestAfter) {
    TestNode nodes[5] = {
        TestNode(50, "Node50"),
        TestNode(30, "Node30"),
        TestNode(70, "Node70"),
        TestNode(20, "Node20"),
        TestNode(80, "Node80")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    TestNode search_key(35, "");
    avl_index_t where;
    TestNode *found = static_cast<TestNode*>(avl_find(&tree, &search_key, &where));
    EXPECT_EQ(found, nullptr);

    TestNode *nearest = static_cast<TestNode*>(avl_nearest(&tree, where, AVL_AFTER));
    ASSERT_NE(nearest, nullptr);
    EXPECT_EQ(nearest->key, 50);
}

// Test: avl_swap
TEST_F(AVLTreeTest, SwapTrees) {
    avl_tree_t tree2;
    avl_create(&tree2, test_node_compare, sizeof(TestNode),
               offsetof(TestNode, avl_link));

    TestNode nodes1[3] = {
        TestNode(10, "Node10"),
        TestNode(20, "Node20"),
        TestNode(30, "Node30")
    };

    TestNode nodes2[2] = {
        TestNode(100, "Node100"),
        TestNode(200, "Node200")
    };

    for (auto &node : nodes1) {
        avl_add(&tree, &node);
    }

    for (auto &node : nodes2) {
        avl_add(&tree2, &node);
    }

    EXPECT_EQ(avl_numnodes(&tree), 3);
    EXPECT_EQ(avl_numnodes(&tree2), 2);

    avl_swap(&tree, &tree2);

    EXPECT_EQ(avl_numnodes(&tree), 2);
    EXPECT_EQ(avl_numnodes(&tree2), 3);

    TestNode *first = static_cast<TestNode*>(avl_first(&tree));
    EXPECT_EQ(first->key, 100);

    first = static_cast<TestNode*>(avl_first(&tree2));
    EXPECT_EQ(first->key, 10);

    // Cleanup tree2
    void *cookie = nullptr;
    while (avl_destroy_nodes(&tree2, &cookie) != nullptr);
    avl_destroy(&tree2);
}

// Test: avl_update
TEST_F(AVLTreeTest, UpdateNode) {
    TestNode nodes[3] = {
        TestNode(20, "Node20"),
        TestNode(10, "Node10"),
        TestNode(30, "Node30")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    // Modify key and update
    nodes[0].key = 25;
    boolean_t updated = avl_update(&tree, &nodes[0]);

    // Update should succeed since order changed
    EXPECT_EQ(updated, B_TRUE);
    VerifyTreeInvariants();
}

// Test: avl_update_lt
TEST_F(AVLTreeTest, UpdateNodeLessThan) {
    TestNode nodes[3] = {
        TestNode(20, "Node20"),
        TestNode(10, "Node10"),
        TestNode(30, "Node30")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    // Decrease key and update (should check previous neighbor)
    nodes[0].key = 15;
    boolean_t updated = avl_update_lt(&tree, &nodes[0]);

    EXPECT_EQ(updated, B_TRUE);
    VerifyTreeInvariants();
}

// Test: avl_update_gt
TEST_F(AVLTreeTest, UpdateNodeGreaterThan) {
    TestNode nodes[3] = {
        TestNode(20, "Node20"),
        TestNode(10, "Node10"),
        TestNode(30, "Node30")
    };

    for (auto &node : nodes) {
        avl_add(&tree, &node);
    }

    // Increase key and update (should check next neighbor)
    nodes[0].key = 25;
    boolean_t updated = avl_update_gt(&tree, &nodes[0]);

    EXPECT_EQ(updated, B_TRUE);
    VerifyTreeInvariants();
}

// Test: Large tree with random insertions
TEST_F(AVLTreeTest, LargeRandomTree) {
    const int NUM_NODES = 1000;
    std::vector<TestNode> nodes;
    nodes.reserve(NUM_NODES);

    // Generate random keys
    std::vector<int> keys;
    for (int i = 0; i < NUM_NODES; ++i) {
        keys.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);

    // Insert nodes
    for (int key : keys) {
        nodes.emplace_back(key, "TestNode");
        avl_add(&tree, &nodes.back());
    }

    EXPECT_EQ(avl_numnodes(&tree), NUM_NODES);
    VerifyTreeInvariants();

    // Verify first and last
    TestNode *first = static_cast<TestNode*>(avl_first(&tree));
    TestNode *last = static_cast<TestNode*>(avl_last(&tree));
    EXPECT_EQ(first->key, 0);
    EXPECT_EQ(last->key, NUM_NODES - 1);
}

// Test: Insert and remove all nodes
TEST_F(AVLTreeTest, InsertRemoveAll) {
    const int NUM_NODES = 100;
    std::vector<TestNode> nodes;
    nodes.reserve(NUM_NODES);

    // Insert nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes.emplace_back(i * 10, "TestNode");
        avl_add(&tree, &nodes.back());
    }

    EXPECT_EQ(avl_numnodes(&tree), NUM_NODES);

    // Remove all nodes
    for (auto &node : nodes) {
        avl_remove(&tree, &node);
    }

    EXPECT_EQ(avl_numnodes(&tree), 0);
    EXPECT_TRUE(avl_is_empty(&tree));
}

// Test: avl_destroy_nodes
TEST_F(AVLTreeTest, DestroyNodes) {
    const int NUM_NODES = 50;
    std::vector<TestNode*> nodes;

    // Dynamically allocate nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        TestNode *node = new TestNode(i * 10, "TestNode");
        nodes.push_back(node);
        avl_add(&tree, node);
    }

    EXPECT_EQ(avl_numnodes(&tree), NUM_NODES);

    // Use avl_destroy_nodes to remove all
    void *cookie = nullptr;
    int count = 0;
    TestNode *node;
    while ((node = static_cast<TestNode*>(avl_destroy_nodes(&tree, &cookie))) != nullptr) {
        count++;
        delete node;
    }

    EXPECT_EQ(count, NUM_NODES);

    // Note: Don't call TearDown cleanup since we already destroyed nodes
}

// Test: TREE_CMP macro
TEST(AVLMacroTest, TreeCmp) {
    EXPECT_EQ(TREE_CMP(5, 10), -1);
    EXPECT_EQ(TREE_CMP(10, 5), 1);
    EXPECT_EQ(TREE_CMP(5, 5), 0);
}

// Test: TREE_ISIGN macro
TEST(AVLMacroTest, TreeIsign) {
    EXPECT_EQ(TREE_ISIGN(-5), -1);
    EXPECT_EQ(TREE_ISIGN(5), 1);
    EXPECT_EQ(TREE_ISIGN(0), 0);
}

// Test: TREE_PCMP macro
TEST(AVLMacroTest, TreePcmp) {
    int a = 10, b = 20;
    EXPECT_EQ(TREE_PCMP(&a, &b), (&a < &b) ? -1 : 1);
    EXPECT_EQ(TREE_PCMP(&a, &a), 0);
}

// Main function
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
