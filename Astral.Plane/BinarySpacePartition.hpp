#pragma once
#include "DenseSet.hpp"
#include "Box.hpp"
#include "List.hpp"
#include "string.hpp"

struct BinaryTreeNode
{
    Box area;
    i32 parentID;
    i32 ID;
    i32 valueIndex;
    i32 childA;
    i32 childB;

    inline BinaryTreeNode()
    {
        this->area = Box();
        this->parentID = 0;
        this->ID = 0;
        this->valueIndex = 0;
        this->childA = 0;
        this->childB = 0;
    }
    inline BinaryTreeNode(i32 parentID, i32 ID, i32 valueIndex, i32 childA, i32 childB)
    {
        this->area = Box();
        this->parentID = parentID;
        this->ID = ID;
        this->valueIndex = valueIndex;
        this->childA = childA;
        this->childB = childB;
    }

    inline bool IsLeaf() const
    {
        return valueIndex != -1;
    }
    inline float SurfaceArea() const
    {
        return area.width * area.height;
    }
};

template <typename T>
struct BinarySpacePartition
{
    IAllocator allocator;
    bool initialized;
    collections::List<i32> freeNodeIndices;
    collections::List<i32> freeValueIndices;
    usize nodesCapacity;
    usize valuesCapacity;

    collections::DenseSet<BinaryTreeNode> nodes;
    collections::DenseSet<T> values;

    i32 trunk;

    inline BinarySpacePartition()
    {
        initialized = false;
        trunk = -1;
        nodesCapacity = 0;
        valuesCapacity = 0;
        freeNodeIndices = collections::List<i32>();
        freeValueIndices = collections::List<i32>();
        nodes = collections::DenseSet<BinaryTreeNode>();
        values = collections::DenseSet<T>();
        allocator = IAllocator{};
    }
    inline BinarySpacePartition(IAllocator allocator)
    {
        initialized = true;
        nodesCapacity = 0;
        valuesCapacity = 0;
        this->allocator = allocator;
        trunk = -1;
        freeNodeIndices = collections::List<i32>(allocator);
        freeValueIndices = collections::List<i32>(allocator);
        nodes = collections::DenseSet<BinaryTreeNode>(allocator, BinaryTreeNode(-1, -1, -1, -1, -1));
        values = collections::DenseSet<T>(allocator);
    }
    inline void deinit()
    {
        freeNodeIndices.deinit();
        freeValueIndices.deinit();
        nodes.deinit();
        values.deinit();
    }

    inline BinaryTreeNode &GetNode(i32 index) const
    {
        return nodes.ptr[index];
    }
    inline i32 GetSibling(i32 index) const
    {
        if (index == trunk)
        {
            return -1;
        }
        const BinaryTreeNode &parentNode = nodes.ptr[nodes.ptr[index].parentID];

        if (parentNode.childA == index)
        {
            return parentNode.childB;
        }
        else
            return parentNode.childA;
    }
    inline i32 GetFreeTreeNodeIndex()
    {
        if (freeNodeIndices.count > 0)
        {
            i32 freeIndex = freeNodeIndices.ptr[freeNodeIndices.count - 1];
            freeNodeIndices.RemoveAt_Pullback(freeNodeIndices.count - 1);
            return freeIndex;
        }

        // force resize array if larger than anticipated
        if (nodesCapacity >= nodes.capacity)
        {
            auto node = BinaryTreeNode();
            node.valueIndex = -1;
            node.parentID = -1;
            node.childA = -1;
            node.childB = -1;
            nodes.Insert((usize)nodesCapacity, node);
        }
        nodes.ptr[nodesCapacity].ID = nodesCapacity;
        nodesCapacity += 1;
        return nodesCapacity - 1;
    }
    inline i32 GetFreeValueIndex()
    {
        if (freeValueIndices.count > 0)
        {
            i32 freeIndex = freeValueIndices.ptr[freeValueIndices.count - 1];
            freeValueIndices.RemoveAt_Pullback(freeValueIndices.count - 1);
            return freeIndex;
        }
        if (valuesCapacity >= values.capacity)
        {
            values.Insert((usize)valuesCapacity, T());
        }
        return valuesCapacity++;
    }

    /// <summary>
    /// Adds and rebalances the tree as necessary, returning an i32 containing the ID of the new node to be stored and used for removal in the future
    /// </summary>
    /// <param name="boundingBox"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    inline i32 Add(Box boundingBox, T value)
    {
        if (trunk == -1)
        {
            BinaryTreeNode &newNode = GetNode(GetFreeTreeNodeIndex());
            newNode.area = boundingBox;
            i32 valueIndex = GetFreeValueIndex();
            newNode.valueIndex = valueIndex;
            values.Insert((usize)valueIndex, value);
            trunk = newNode.ID;

            return newNode.ID;
        }
        return RecursiveAdd(boundingBox, trunk, value);
    }
    inline void CheckSwap(i32 nodeIndex)
    {
        auto leafNode = GetNode(nodeIndex);
        if (leafNode.parentID == -1 || GetNode(leafNode.parentID).parentID == -1)
        {
            // cant swap anything
            return;
        }
        i32 siblingOfParent = GetSibling(leafNode.parentID);
        i32 mySibling = GetSibling(nodeIndex);

        float currentSurfaceArea = nodes.ptr[leafNode.parentID].SurfaceArea();
        Box areaIfSwapped = Box::Union(nodes.ptr[mySibling].area, nodes.ptr[siblingOfParent].area);
        float surfaceAreaIfSwapped = areaIfSwapped.SurfaceArea();

        if (surfaceAreaIfSwapped < currentSurfaceArea)
        {
            i32 grandparentID = GetNode(leafNode.parentID).parentID;
            nodes.ptr[siblingOfParent].parentID = leafNode.parentID;
            nodes.ptr[nodeIndex].parentID = grandparentID;

            if (nodes.ptr[grandparentID].childA == siblingOfParent)
            {
                nodes.ptr[grandparentID].childA = nodeIndex;
            }
            else
            {
                nodes.ptr[grandparentID].childB = nodeIndex;
            }

            if (nodes.ptr[leafNode.parentID].childA == nodeIndex)
            {
                nodes.ptr[leafNode.parentID].childA = siblingOfParent;
            }
            else
            {
                nodes.ptr[leafNode.parentID].childB = siblingOfParent;
            }
            nodes.ptr[leafNode.parentID].area = areaIfSwapped;
        }
    }
    inline i32 RecursiveAdd(Box boundingBox, i32 originalNodeID, const T &value)
    {
        auto originalNode = nodes.ptr[originalNodeID];
        if (originalNode.IsLeaf())
        {
            // parent is leaf, change parent into a branch
            // add parent's original data as a new leaf, A
            // Error: Cannot change parent into a branch as that would mutate parent, which may be
            // held externally, and would have expected parent to stay as a leaf so as to perform
            // remove query on it later!
            // Solution: Create a new node that is a branch, and set our parent node as a leaf of this
            // new branch, which would take our parent former-leaf's place
            BinaryTreeNode newBranch = GetNode(GetFreeTreeNodeIndex());
            BinaryTreeNode newLeafB = GetNode(GetFreeTreeNodeIndex());

            i32 newLeafBValueIndex = GetFreeValueIndex();
            values.Insert((usize)newLeafBValueIndex, value);

            newLeafB.area = boundingBox;
            newLeafB.valueIndex = newLeafBValueIndex;
            newLeafB.parentID = newBranch.ID;

            i32 originalNodeParentID = originalNode.parentID;
            originalNode.parentID = newBranch.ID;
            newBranch.parentID = originalNodeParentID;

            newBranch.childA = originalNode.ID;
            newBranch.childB = newLeafB.ID;

            if (newBranch.parentID == -1)
            {
                trunk = newBranch.ID;
            }
            else
            {
                if (nodes.ptr[originalNodeParentID].childA == originalNode.ID)
                {
                    nodes.ptr[originalNodeParentID].childA = newBranch.ID;
                }
                else
                {
                    nodes.ptr[originalNodeParentID].childB = newBranch.ID;
                }
            }

            nodes.Insert((usize)newBranch.ID, newBranch);
            nodes.Insert((usize)newLeafB.ID, newLeafB);
            nodes.Insert((usize)originalNode.ID, originalNode);

            // recursively set surface area and check if we should swap
            i32 index = newBranch.ID;
            while (index != -1)
            {
                i32 childA = GetNode(index).childA;
                i32 childB = GetNode(index).childB;
                nodes.ptr[index].area = Box::Union(GetNode(childA).area, GetNode(childB).area);

                CheckSwap(childA);
                CheckSwap(childB);

                index = nodes.ptr[index].parentID;
            }

            return newLeafB.ID;
        }
        else // is branch
        {
            /*
            get the surface area of the union of bounding boxes
            between the leaf and the new data's bounding box.
            The one with the smaller surface area upon the union
            will be the selected leaf to turn into a branch
            if it is a branch instead, the process continues unto infinity until a leaf is encountered
            */
            float nodeASurface = Box::Union(boundingBox, GetNode(originalNode.childA).area).SurfaceArea();
            float nodeBSurface = Box::Union(boundingBox, GetNode(originalNode.childB).area).SurfaceArea();

            if (nodeASurface <= nodeBSurface)
            {
                return RecursiveAdd(boundingBox, originalNode.childA, value);
            }
            else
                return RecursiveAdd(boundingBox, originalNode.childB, value);
        }
    }
    inline void Remove(i32 leafID)
    {
        if (!nodes.ptr[leafID].IsLeaf())
        {
            // Attempted to remove a branch!
            return;
        }
        if (leafID == trunk)
        {
            trunk = -1;
            i32 valueIndex = nodes.ptr[leafID].valueIndex;
            values.Remove((u32)valueIndex);
            freeValueIndices.Add(valueIndex);

            nodes.ptr[leafID].valueIndex = -1;
            nodes.ptr[leafID].area = Box();
            freeNodeIndices.Add(leafID);
        }
        else
        {
            // remove leaf, set sibling to parent to node's grandparent
            i32 siblingID = GetSibling(leafID);
            i32 parentID = nodes.ptr[leafID].parentID;
            i32 grandparentID = nodes.ptr[parentID].parentID;
            nodes.ptr[siblingID].parentID = grandparentID;

            if (grandparentID != -1)
            {
                if (nodes.ptr[grandparentID].childA == parentID)
                {
                    nodes.ptr[grandparentID].childA = siblingID;
                }
                else
                {
                    nodes.ptr[grandparentID].childB = siblingID;
                }
            }
            else
            {
                trunk = siblingID;
            }

            // original parent is now obsolete as it was a branch that has since been merged
            // original leaf is also removed
            // add both to the freenodes pile

            i32 valueIndex = nodes.ptr[leafID].valueIndex;
            values.Remove((u32)valueIndex);
            freeValueIndices.Add(valueIndex);

            nodes.ptr[leafID].area = Box();
            nodes.ptr[leafID].valueIndex = -1;
            freeNodeIndices.Add(leafID);
            nodes.ptr[parentID].area = Box();
            nodes.ptr[parentID].childA = -1;
            nodes.ptr[parentID].childB = -1;
            nodes.ptr[parentID].parentID = -1;
            freeNodeIndices.Add(parentID);

            // once we are done, ensure tree fits properly
            i32 index = grandparentID;
            while (index != -1)
            {
                i32 childA = GetNode(index).childA;
                i32 childB = GetNode(index).childB;
                nodes.ptr[index].area = Box::Union(GetNode(childA).area, GetNode(childB).area);

                index = nodes.ptr[index].parentID;
            }
        }
    }
    inline i32 Update(i32 leafID, Box newBounds)
    {
        if (nodes.ptr[leafID].area.Inflate(0.01f, 0.01f).Contains(newBounds))
        {
            return -1;
        }
        T value = values.ptr[(u32)nodes.ptr[leafID].valueIndex];
        Remove(leafID);
        i32 result = Add(newBounds, value);
        if (result != leafID)
        {
            // exception
            return -1;
        }
        return result;
    }
    /*
    // all in c#

    inline BinaryTreeQuery<T> Query(Box queryRegion)
    {
        BinaryTreeQuery<T> result = new BinaryTreeQuery<T>(this);
        result.Execute(queryRegion);
        return result;
    }
    inline BinaryTreeQuery<T> Query(RaycastInput queryRaycast)
        {
            BinaryTreeQuery<T> result = new BinaryTreeQuery<T>(this);
            result.Execute(queryRaycast);
            return result;
        }
    */
    inline string GetNodeAsString(IAllocator allocator, i32 node) const
    {
        if (GetNode(node).IsLeaf())
        {
            return string::Format(allocator, "Leaf %i", node);
        }
        else
        {
            return string::Format(allocator, "Branch %i", node);
        }
    }
};

#define BINARYTREEQUERY_POOL_MAX_ITEMS 32
#define BINARYTREEQUERY_RESULTS_MAX_ITEMS 32

template <typename T>
struct BinaryTreeQuery
{
    struct Pair
    {
        T result;
        i32 index;
    };

    const BinarySpacePartition<T> *tree;
    Pair results[32];
    i32 resultsCount;
    //collections::List<Pair> results;
    i32 index;

    inline T Get(usize i)
    {
        return results[i].result;
    }
    inline T &GetRef(usize i)
    {
        return results[i].result;
    }
    inline i32 GetNodeIndexOf(usize i)
    {
        return results[i].index;
    }

    inline BinaryTreeQuery(BinarySpacePartition<T> *tree)
    {
        this->tree = tree;
        index = -1;
    }

    /*void Execute(Raycast2D input)
    {
        resultsCount = 0;
        i32 pool[BINARYTREEQUERY_POOL_MAX_ITEMS];
        pool[0] = tree->trunk;
        i32 poolCount = 1;
        //collections::List<i32> pool = collections::List<i32>(allocator); // acts as a stack
        //pool.Add(tree->trunk);

        while (poolCount > 0) //(pool.TryPop(out var index))//stackCount > 0)
        {
            // pops the pool
            i32 index = pool[poolCount - 1];
            poolCount--;

            if (index == -1 || index >= tree->nodes.capacity)
            {
                continue;
            }
            if (poolCount > BINARYTREEQUERY_POOL_MAX_ITEMS)
            {
                poolCount = BINARYTREEQUERY_POOL_MAX_ITEMS;
            }
            if (tree->nodes.ptr[index].IsLeaf())
            {
                RaycastHit2D discard;
                if (Intersections::RaycastBox(raycast, tree->nodes.ptr[index].area, QueryPosition2D(), &discard)) // queryAt.Intersects(tree.nodes.values[index].area))
                {
                    results[resultsCount++] = {tree->values.ptr[(u32)tree->nodes.ptr[index].valueIndex], index};
                    if (resultsCount >= 8)
                    {
                        return;
                    }
                }
            }
            else
            {
                RaycastHit2D discard;
                if (Intersections::RaycastBox(raycast, tree->nodes.ptr[tree->nodes.ptr[index].childA].area, QueryPosition2D(), &discard)) // queryAt.Intersects(tree.nodes.values[tree.nodes.values[index].childA].area))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childA;
                }
                if (Intersections::RaycastBox(raycast, tree->nodes.ptr[tree->nodes.ptr[index].childB].area, QueryPosition2D(), &discard))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childB;
                }
            }
        }
    }*/

    void Execute(Maths::Vec2 point)
    {
        resultsCount = 0;
        i32 pool[BINARYTREEQUERY_POOL_MAX_ITEMS];
        pool[0] = tree->trunk;
        i32 poolCount = 1;

        while (poolCount > 0)
        {
            // pops the pool
            i32 index = pool[poolCount - 1];
            poolCount--;

            if (index == -1 || index >= tree->nodes.capacity)
            {
                continue;
            }
            if (poolCount > BINARYTREEQUERY_POOL_MAX_ITEMS)
            {
                poolCount = BINARYTREEQUERY_POOL_MAX_ITEMS;
            }
            if (tree->nodes.ptr[index].IsLeaf())
            {
                if (tree->nodes.ptr[index].area.Contains(point))
                {
                    results[resultsCount++] = {tree->values.ptr[(u32)tree->nodes.ptr[index].valueIndex], index};
                    if (resultsCount >= BINARYTREEQUERY_RESULTS_MAX_ITEMS)
                    {
                        return;
                    }
                }
            }
            else
            {
                if (tree->nodes.ptr[tree->nodes.ptr[index].childA].area.Contains(point))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childA;
                }
                if (tree->nodes.ptr[tree->nodes.ptr[index].childB].area.Contains(point))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childB;
                }
            }
        }
    }
    void Execute(Maths::Vec2 point1, Maths::Vec2 point2)
    {
        resultsCount = 0;
        i32 pool[BINARYTREEQUERY_POOL_MAX_ITEMS];
        pool[0] = tree->trunk;
        i32 poolCount = 1;

        while (poolCount > 0)
        {
            // pops the pool
            i32 index = pool[poolCount - 1];
            poolCount--;

            if (index == -1 || index >= tree->nodes.capacity)
            {
                continue;
            }
            if (poolCount > BINARYTREEQUERY_POOL_MAX_ITEMS)
            {
                poolCount = BINARYTREEQUERY_POOL_MAX_ITEMS;
            }
            if (tree->nodes.ptr[index].IsLeaf())
            {
                const Box &area = tree->nodes.ptr[index].area;
                if (area.Contains(point1) && area.Contains(point2))
                {
                    results[resultsCount++] = {tree->values.ptr[(u32)tree->nodes.ptr[index].valueIndex], index};
                    if (resultsCount >= BINARYTREEQUERY_RESULTS_MAX_ITEMS)
                    {
                        return;
                    }
                }
            }
            else
            {
                const Box &area1 = tree->nodes.ptr[tree->nodes.ptr[index].childA].area;
                const Box &area2 = tree->nodes.ptr[tree->nodes.ptr[index].childB].area;
                if (area1.Contains(point1) && area1.Contains(point2))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childA;
                }
                if (area2.Contains(point1) && area2.Contains(point2))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childB;
                }
            }
        }
    }
    void Execute(Box queryAt)
    {
        resultsCount = 0;
        i32 pool[BINARYTREEQUERY_POOL_MAX_ITEMS];
        pool[0] = tree->trunk;
        i32 poolCount = 1;

        while (poolCount > 0) //(pool.TryPop(out var index))//stackCount > 0)
        {
            // pops the pool
            i32 index = pool[poolCount - 1];
            poolCount--;

            if (index == -1 || index >= tree->nodes.capacity)
            {
                continue;
            }
            if (poolCount > BINARYTREEQUERY_POOL_MAX_ITEMS)
            {
                poolCount = BINARYTREEQUERY_POOL_MAX_ITEMS;
            }
            if (tree->nodes.ptr[index].IsLeaf())
            {
                if (queryAt.Intersects(tree->nodes.ptr[index].area))
                {
                    results[resultsCount++] = {tree->values.ptr[(u32)tree->nodes.ptr[index].valueIndex], index};
                    if (resultsCount >= BINARYTREEQUERY_RESULTS_MAX_ITEMS)
                    {
                        return;
                    }
                }
            }
            else
            {
                if (queryAt.Intersects(tree->nodes.ptr[tree->nodes.ptr[index].childA].area))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childA;
                }
                if (queryAt.Intersects(tree->nodes.ptr[tree->nodes.ptr[index].childB].area))
                {
                    pool[poolCount++] = tree->nodes.ptr[index].childB;
                }
            }
        }
    }
};