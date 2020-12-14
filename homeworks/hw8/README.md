# 作业 8

## 任务

实现平面点集 CVT 的 Lloyd 算法

## 目的

学习 Voronoi 算法、使用相关数学库（如 [Triangle](http://www.cs.cmu.edu/~quake/triangle.html)、[CGAL](https://www.cgal.org/) 等）

## 步骤

- 在给定的正方形区域内随机生成若干采样点
- 生成这些点的 Voronoi 剖分
- 计算每个剖分的中心，将采样点的位置更新到重心
- 迭代步骤 2 和 3

## 思考（非必要、可选）

如何在曲面上采样并生成 CVT（如何将算法推广到流形曲面）？

## Deadline

2020.12.19