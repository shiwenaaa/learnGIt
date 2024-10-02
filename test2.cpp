/*
* 169. 多数元素
* 给定一个大小为 n 的数组 nums ，返回其中的多数元素。多数元素是指在数组中出现次数 大于 ⌊ n/2 ⌋ 的元素。
你可以假设数组是非空的，并且给定的数组总是存在多数元素。

示例 1：
输入：nums = [3,2,3]
输出：3

示例 2：
输入：nums = [2,2,1,1,1,2,2]
输出：2
*/

// 解题思路：使用摩尔投票算法
/*摩尔投票算法是一种用于查找数组中多数元素的有效算法。多数元素是指在数组中出现次数超过 ⌊n/2⌋ 的元素。
该算法的基本思想是通过维护一个候选元素和一个计数器来逐步确定这个元素。

算法步骤：
1、初始化：
设置一个候选元素 candidate 和一个计数器 count，初始值分别为任意元素和 0。

2、遍历数组：
对于数组中的每一个元素：
如果 count 为 0，则将当前元素设为新的 candidate，并将 count 设置为 1。
如果当前元素与 candidate 相同，增加 count。
如果当前元素与 candidate 不同，减少 count。

3、最终结果：
遍历结束时，candidate 即为数组中的多数元素。

4、特点：
时间复杂度：O(n)，只需一次遍历。
空间复杂度：O(1)，只使用常数空间。
适用条件：适用于保证存在多数元素的情况。

示例：
考虑数组 [2, 2, 1, 1, 1, 2, 2]：
①初始化：candidate = 2, count = 0
②遍历：
2 -> count = 1
2 -> count = 2
1 -> count = 1
1 -> count = 0
1 -> count = 1
2 -> count = 0
2 -> count = 1
最后得到的 candidate 为 2，正是多数元素。

摩尔投票算法非常高效且简单，是解决该问题的一种经典方法。
*/

#include <vector>
using namespace std;

class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int count = 0, candidate;

        for (int num : nums) {
            if (count == 0) {
                candidate = num;
            }
            count += (num == candidate) ? 1 : -1;
        }

        return candidate;
    }
};