#pragma once
#include "Scope.hpp"
#include "Box.hpp"
#include "BinarySpacePartition.hpp"

/// @brief cuts a hole into the box 'input' with the given clipping area 'clip.' This results in at
/// most, 4 outputted boxes if the clip is contained by the input.
inline bool Partitions2D_ClipBoxBox(Box input, Box clip, Box *outputs, u32 *numOutputs)
{
    const Maths::Vec2 inputTopLeft = input.GetTopLeft();
    const Maths::Vec2 inputBtmRight = input.GetBottomRight();
    const Maths::Vec2 clipTopLeft = clip.GetTopLeft();
    const Maths::Vec2 clipBtmRight = clip.GetBottomRight();

    //clip is fully within input
    if (inputTopLeft < clipTopLeft && inputBtmRight > clipBtmRight)
    {
        //1223
        //1--3
        //1--3
        //1443

        //fully contained = 4 outputs
        *numOutputs = 4;
        //left
        outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, clipTopLeft.X - inputTopLeft.X, input.height);
        //top
        outputs[1] = Box(clipTopLeft.X, inputTopLeft.Y, clip.width, clipTopLeft.Y - inputTopLeft.Y);
        //right
        outputs[2] = Box(clipBtmRight.X, inputTopLeft.Y, inputBtmRight.X - clipBtmRight.X, input.height);
        //bottom
        outputs[3] = Box(clipTopLeft.X, clipBtmRight.Y, clip.width, inputBtmRight.Y - clipBtmRight.Y);
        return true;
    }
    else
    {
        bool topLeftClipped = clip.Contains(inputTopLeft);
        bool topRightClipped = clip.Contains(Maths::Vec2(inputBtmRight.X, inputTopLeft.Y));
        bool btmRightClipped = clip.Contains(inputBtmRight);
        bool btmLeftClipped = clip.Contains(Maths::Vec2(inputTopLeft.X, inputBtmRight.Y));

        u32 total = (u32)topLeftClipped + (u32)topRightClipped + (u32)btmRightClipped + (u32)btmLeftClipped;
        //clip will cut off two of the edges of the input
        if (total == 4)
        {
            //All four corners are clipped, thus no output box is yielded
            *numOutputs = 0;
            return true;
        }
        // else if (total == 3)
        // {
            // this is logically impossible since neither box are rotated
        // }
        else if (total == 2)
        {
            //2 edges clipped, a side of the box is sheared off
            *numOutputs = 1;
            if (topLeftClipped && topRightClipped)
            {
                //---
                //111
                //111
                //top edge clipped
                outputs[0] = Box(inputTopLeft.X, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
            }
            else if (topRightClipped && btmRightClipped)
            {
                //11-
                //11-
                //11-
                //right edge clipped
                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, clipTopLeft.X - inputTopLeft.X, input.height);
            }
            else if (btmRightClipped && btmLeftClipped)
            {
                //111
                //111
                //---
                //bottom edge clipped
                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipTopLeft.Y - inputTopLeft.X);
            }
            else
            {
                //-11
                //-11
                //-11
                //left edge clipped
                outputs[0] = Box(clipBtmRight.X, inputTopLeft.Y, inputBtmRight.X - clipBtmRight.X, input.height);
            }
            return true;
        }
        else if (total == 1)
        {
            //one edge is clipped
            *numOutputs = 2;
            if (topLeftClipped)
            {
                // --11
                // --11
                // 2222
                // 2222

                outputs[0] = Box(clipBtmRight.X, inputTopLeft.Y, inputBtmRight.X - clipBtmRight.X, clipBtmRight.Y - inputTopLeft.Y);
                outputs[1] = Box(inputTopLeft.X, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
            }
            else if (topRightClipped)
            {
                // 11--
                // 11--
                // 2222
                // 2222

                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, clipTopLeft.X - inputTopLeft.X, clipBtmRight.Y - inputTopLeft.Y);
                outputs[1] = Box(inputTopLeft.X, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
            }
            else if (btmRightClipped)
            {
                // 1111
                // 1111
                // 22--
                // 22--

                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipBtmRight.Y - inputTopLeft.Y);
                outputs[1] = Box(inputTopLeft.X, clipBtmRight.Y, clipTopLeft.X - inputTopLeft.X, inputBtmRight.Y - clipBtmRight.Y);
            }
            else
            {
                // 1111
                // 1111
                // --22
                // --22
                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipBtmRight.Y - inputTopLeft.Y);
                outputs[1] = Box(clipBtmRight.X, clipTopLeft.Y, inputBtmRight.X - clipBtmRight.X, inputBtmRight.Y - clipTopLeft.Y);
            }
            return true;
        }
        else
        {
            const float left = inputTopLeft.X;
            const float top = inputTopLeft.Y;
            const float right = inputBtmRight.X;
            const float bottom = inputBtmRight.Y;

            const float clipLeft = clipTopLeft.X;
            const float clipTop = clipTopLeft.Y;
            const float clipRight = clipBtmRight.X;
            const float clipBottom = clipBtmRight.Y;

            //box is clipped like a jigsaw
            if (clipLeft <= right && clipRight >= left && clipTop <= bottom && clipBottom >= top)
            {
                if (clipRight > right)
                {
                    // 1111
                    // 22--
                    // 22--
                    // 3333
                    *numOutputs = 3;
                    outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipTopLeft.Y - inputTopLeft.Y);
                    outputs[1] = Box(inputTopLeft.X, clipTopLeft.Y, clipTopLeft.X - inputTopLeft.X, clip.height);
                    outputs[2] = Box(inputTopLeft.Y, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
                    return true;
                }
                else if (clipLeft < left)
                {
                    // 1111
                    // --22
                    // --22
                    // 3333
                    *numOutputs = 3;
                    outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipTopLeft.Y - inputTopLeft.Y);
                    outputs[1] = Box(clipBtmRight.X, clipTopLeft.Y, clip.height, inputBtmRight.X - clipBtmRight.X);
                    outputs[2] = Box(inputTopLeft.Y, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
                    return true;
                }
                else if (clipTop < top)
                {
                    // 1--3
                    // 1--3
                    // 1223
                    // 1223
                    *numOutputs = 3;
                    outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, clipTopLeft.X - inputTopLeft.X, input.height);
                    outputs[1] = Box(clipTopLeft.X, clipBtmRight.Y, clip.width, inputBtmRight.Y - clipBtmRight.Y);
                    outputs[2] = Box(clipBtmRight.X, inputTopLeft.Y, inputBtmRight.X - clipBtmRight.X, input.height);
                    return true;
                }
                else if (clipBottom > bottom)
                {
                    // 1223
                    // 1223
                    // 1--3
                    // 1--3
                    *numOutputs = 3;
                    outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, clipTopLeft.X - inputTopLeft.X, input.height);
                    outputs[1] = Box(clipTopLeft.X, inputTopLeft.Y, clip.width, clipTopLeft.Y - inputTopLeft.Y);
                    outputs[2] = Box(clipBtmRight.X, inputTopLeft.Y, inputBtmRight.X - clipBtmRight.X, input.height);
                    return true;
                }
                else
                {
                    // anomalous results
                    *numOutputs = 0;
                    return false;
                }
            }
            //no intersection
            else
            {
                *numOutputs = 0;
                return false;
            }
        }
        *numOutputs = 0;
        return false;
    }
}

inline void Partitions2D_MergeBoxes_Horizontal(const Box *scanFrom, u32 scanCount, u8 *mergedIntoStates, float allowedApproximation, collections::List<Box> &outputs)
{
    if (scanCount == 1)
    {
        outputs.Add(scanFrom[0]);
        return;
    }
    // const Box *scanFrom = inputs;
    // u32 scanCount = inputsCount;
    BinarySpacePartition<u8 *> boxes = BinarySpacePartition<u8 *>(GetCAllocator());
    Scope(BinarySpacePartition<u8*>, boxes);
    //we need to be able to query input index to mergedIntoStates;
    for (u32 i = 0; i < scanCount; i++)
    {
        //false = not checked yet
        mergedIntoStates[i] = 0;
        boxes.Add(scanFrom[i], &mergedIntoStates[i]);
    }

    for (u32 i = 0; i < scanCount; i++)
    {
        Box input = scanFrom[i];
        u8 *thisMergedInto = &mergedIntoStates[i];
        if (*thisMergedInto != 0)
        {
            continue;
        }
        bool add = false;

        //merge right
        bool shouldContinue = true;
        while (shouldContinue)
        {
            BinaryTreeQuery<u8 *> query = BinaryTreeQuery<u8 *>(&boxes);
            query.Execute(input.GetTopRight() + Maths::Vec2(allowedApproximation * 1.5f, 0.001f), input.GetBottomRight() + Maths::Vec2(allowedApproximation * 1.5f, -0.001f));

            shouldContinue = false;
            for (u32 j = 0; j < query.resultsCount; j++)
            {
                i32 nodeIndex = query.GetNodeIndexOf(j);
                u8* otherMergedInto = query.Get(j);
                Box resultArea = boxes.GetNode(nodeIndex).area;

                i8 touchingEdge = input.Touches(resultArea, 0.001f);

                if (touchingEdge == 2)
                {
                    if (fabsf(input.GetTop() - resultArea.GetTop()) <= allowedApproximation && fabsf(input.GetBottom() - resultArea.GetBottom()) <= allowedApproximation)
                    {
                        *otherMergedInto = 1;
                        *thisMergedInto = 1;

                        input = Box(input.X, input.Y, input.width + resultArea.width, input.height);
                        add = true;
                        //keep merging right
                        shouldContinue = true;
                        break;
                    }
                }
            }
        }
        //merge left
        shouldContinue = true;
        while (shouldContinue)
        {
            BinaryTreeQuery<u8 *> query = BinaryTreeQuery<u8 *>(&boxes);
            query.Execute(input.GetTopLeft() - Maths::Vec2(allowedApproximation * 1.5f, -0.001f), input.GetBottomLeft() - Maths::Vec2(allowedApproximation * 1.5f, 0.001f));

            shouldContinue = false;
            for (u32 j = 0; j < query.resultsCount; j++)
            {
                i32 nodeIndex = query.GetNodeIndexOf(j);
                u8* otherMergedInto = query.Get(j);
                Box resultArea = boxes.GetNode(nodeIndex).area;

                i8 touchingEdge = input.Touches(resultArea, 0.001f);
                if (touchingEdge == 0)
                {
                    if (fabsf(input.GetTop() - resultArea.GetTop()) <= allowedApproximation && fabsf(input.GetBottom() - resultArea.GetBottom()) <= allowedApproximation)
                    {
                        *otherMergedInto = 1;
                        *thisMergedInto = 1;

                        input = Box(resultArea.X, input.Y, resultArea.width + input.width, input.height);
                        add = true;
                        //keep merging left
                        shouldContinue = true;
                        break;
                    }
                }
            }
        }

        if (add)
        {
            outputs.Add(input);
        }
    }
    for (u32 i = 0; i < scanCount; i++)
    {
        //output unmerged boxes
        if (mergedIntoStates[i] == 0)
        {
            outputs.Add(scanFrom[i]);
        }
    }
}
inline void Partitions2D_MergeBoxes_Vertical(const Box *scanFrom, u32 scanCount, u8 *mergedIntoStates, float allowedApproximation, collections::List<Box> &outputs)
{
    if (scanCount == 1)
    {
        outputs.Add(scanFrom[0]);
        return;
    }
    BinarySpacePartition<u8 *> boxes = BinarySpacePartition<u8 *>(GetCAllocator());
    Scope(BinarySpacePartition<u8*>, boxes);
    //we need to be able to query input index to mergedIntoStates;
    for (u32 i = 0; i < scanCount; i++)
    {
        //false = not checked yet
        mergedIntoStates[i] = 0;
        boxes.Add(scanFrom[i], &mergedIntoStates[i]);
    }

    for (u32 i = 0; i < scanCount; i++)
    {
        Box input = scanFrom[i];
        u8 *thisMergedInto = &mergedIntoStates[i];
        if (*thisMergedInto != 0)
        {
            continue;
        }
        bool add = false;

        //merge down
        bool shouldContinue = true;
        while (shouldContinue)
        {
            BinaryTreeQuery<u8 *> query = BinaryTreeQuery<u8 *>(&boxes);
            query.Execute(input.GetBottomLeft() + Maths::Vec2(0.001f, allowedApproximation * 1.5f), input.GetBottomRight() + Maths::Vec2(-0.001f, allowedApproximation * 1.5f));

            shouldContinue = false;
            for (u32 j = 0; j < query.resultsCount; j++)
            {
                i32 nodeIndex = query.GetNodeIndexOf(j);
                u8* otherMergedInto = query.Get(j);
                Box resultArea = boxes.GetNode(nodeIndex).area;

                i8 touchingEdge = input.Touches(resultArea, 0.001f);

                if (touchingEdge == 3)
                {
                    if (fabsf(input.GetLeft() - resultArea.GetLeft()) <= allowedApproximation && fabsf(input.GetRight() - resultArea.GetRight()) <= allowedApproximation)
                    {
                        *otherMergedInto = 1;
                        *thisMergedInto = 1;

                        input = Box(input.X, input.Y, input.width, input.height + resultArea.height);
                        add = true;
                        //keep merging down
                        shouldContinue = true;
                        break;
                    }
                }
            }
        }

        //merge up
        shouldContinue = true;
        while (shouldContinue)
        {
            BinaryTreeQuery<u8 *> query = BinaryTreeQuery<u8 *>(&boxes);
            query.Execute(input.GetBottomLeft() - Maths::Vec2(-0.001f, allowedApproximation * 1.5f), input.GetBottomRight() - Maths::Vec2(0.001f, allowedApproximation * 1.5f));

            shouldContinue = false;
            for (u32 j = 0; j < query.resultsCount; j++)
            {
                i32 nodeIndex = query.GetNodeIndexOf(j);
                u8* otherMergedInto = query.Get(j);
                Box resultArea = boxes.GetNode(nodeIndex).area;

                i8 touchingEdge = input.Touches(resultArea, 0.001f);
                if (touchingEdge == 1)
                {
                    if (fabsf(input.GetTop() - resultArea.GetTop()) <= allowedApproximation && fabsf(input.GetBottom() - resultArea.GetBottom()) <= allowedApproximation)
                    {
                        *otherMergedInto = 1;
                        *thisMergedInto = 1;

                        input = Box(input.X, resultArea.Y, input.width, resultArea.height + input.height);
                        add = true;
                        //keep merging up
                        shouldContinue = true;
                        break;
                    }
                }
            }
        }

        if (add)
        {
            outputs.Add(input);
        }
    }
    for (u32 i = 0; i < scanCount; i++)
    {
        //output unmerged boxes
        if (mergedIntoStates[i] == 0)
        {
            outputs.Add(scanFrom[i]);
        }
    }
}
inline void Partitions2D_MergeBoxes(const Box *inputs, u32 inputsCount, collections::List<Box> &outputs, float allowedApproximation, bool mergeHorizontal, bool mergeVertical)
{
    u8 *mergedIntoStates = (u8 *)DEFAULT_ALLOC(sizeof(u8) * inputsCount);
    collections::List<Box> intermediateOutputs = {};
    if (mergeHorizontal && mergeVertical)
    {
        intermediateOutputs = collections::List<Box>(GetCAllocator());
    }
    //merge horizontal
    if (mergeHorizontal)
    {
        if (mergeVertical)
        {
            Partitions2D_MergeBoxes_Horizontal(inputs, inputsCount, mergedIntoStates, allowedApproximation, intermediateOutputs);
        }
        else
        {
            Partitions2D_MergeBoxes_Horizontal(inputs, inputsCount, mergedIntoStates, allowedApproximation, outputs);
            return;
        }
    }
    
    u32 prevOutputs = outputs.count;
    //merge vertical
    if (mergeVertical)
    {
        if (mergeHorizontal)
        {
            memset(mergedIntoStates, 0, intermediateOutputs.count * sizeof(u8));
            Partitions2D_MergeBoxes_Vertical(intermediateOutputs.ptr, intermediateOutputs.count, mergedIntoStates, allowedApproximation, outputs);
        }
        else
        {
            //if we are not merging horizontal, then intermediateOutputs would be empty, so we
            //instead read directly from inputs.
            Partitions2D_MergeBoxes_Vertical(inputs, inputsCount, mergedIntoStates, allowedApproximation, outputs);
            return;
        }
    }
    if (mergeHorizontal && mergeVertical)
    {
        //If we are merging both horizontal and vertical, there needs to be a final horizontal pass
        //for cases where boxes are only able to be merged horizontally after two prior boxes are
        //merged vertically.

        //In order to accomplish this, we copy the current data in outputs back
        //to intermediateOutputs, clear outputs, then run the first procedure again.
        intermediateOutputs.Clear();
        for (u32 i = prevOutputs; i < outputs.count; i++)
        {
            intermediateOutputs.Add(outputs[i]);
        }
        outputs.count = prevOutputs;
        memset(mergedIntoStates, 0, intermediateOutputs.count * sizeof(u8));

        Partitions2D_MergeBoxes_Horizontal(intermediateOutputs.ptr, intermediateOutputs.count, mergedIntoStates, allowedApproximation, outputs);

        //lastly, we deinitt he intermediate output data
        intermediateOutputs.deinit();
    }
    DEFAULT_FREE(mergedIntoStates);
}