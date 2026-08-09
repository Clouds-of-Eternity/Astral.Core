#pragma once
#include "Box.hpp"

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
                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, inputBtmRight.X - inputTopLeft.X, input.height);
            }
            else if (btmRightClipped && btmLeftClipped)
            {
                //111
                //111
                //---
                //bottom edge clipped
                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipBtmRight.Y - inputTopLeft.X);
            }
            else
            {
                //-11
                //-11
                //-11
                //left edge clipped
                outputs[0] = Box(clipTopLeft.X, inputTopLeft.Y, inputBtmRight.X - clipTopLeft.X, input.height);
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
            // intersects = !(
            //     other.left > right ||
            //     other.right < left ||
            //     other.top > bottom ||
            //     other.bottom < top);
            //
            // = other.left <= right && other.right >= left && other.top <= bottom && other.bottom >= top

            const float left = inputTopLeft.X;
            const float top = inputTopLeft.Y;
            const float right = inputBtmRight.X;
            const float bottom = inputBtmRight.Y;

            const float clipLeft = clipTopLeft.X;
            const float clipTop = clipTopLeft.Y;
            const float clipRight = clipBtmRight.X;
            const float clipBottom = clipBtmRight.Y;

            //box is clipped like a jigsaw
            if (clipLeft <= right)
            {
                *numOutputs = 3;
                // 1111
                // 22--
                // 22--
                // 3333

                outputs[0] = Box(inputTopLeft.X, inputTopLeft.Y, input.width, clipTopLeft.Y - inputTopLeft.Y);
                outputs[1] = Box(clipBtmRight.X, clipTopLeft.Y, inputBtmRight.X - clipBtmRight.X, clip.height);
                outputs[2] = Box(inputTopLeft.X, clipBtmRight.Y, input.width, inputBtmRight.Y - clipBtmRight.Y);
            }
            else if (clipRight >= left)
            {
                *numOutputs = 3;
                // 1111
                // --22
                // --22
                // 3333
            }
            //no intersection
            else
            {
                *numOutputs = 0;
                return false;
            }
        }
    }
}