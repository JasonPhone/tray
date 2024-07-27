# 加载器

## 动机

需要一个方法来加载渲染器的配置和场景信息。

## 方法

将人类可读的（但愿）设置文件映射到渲染器和场景中的各个对象。

参数集合，仍然使用 JSON。

每个文件一个场景，每个场景包含：

- 渲染器选项：
  - n_threads
  - verbose level
- 场景配置：
  - 一个或多个
    - transforms, shapes
    - colors, textures, materials
    - lights, primitives.
  - 单个
    - accelerator, camera
    - sampler, integrator.

### 配置文件语法

完整文件：

```json
{
  "comment": "an example scene file",

  "lists of multiple objects": "below",
  "transforms": [],
  "colors": [],
  "textures": [],
  "materials": [],
  "lights": [],
  "primitives": [],

  "single objects":"below",
  "accelerator": {},
  "camera": {},
  "sampler": {},
  "integrator": {},
  "render": {},

  "end":{}
}
```

单个对象：

```json
{
  "name": "used for key-val mapping",
  "type": "",
  "reference_param": "some defined object",
  "single value": 42,
  "multiple values": [1, 2, 3],
  "vertices": [
    [0, 0, 0],
    [1, 1, 1]
  ],
  "boolean": false,
  "end": ""
}
```

### 代码设计

`Params`: Parameters, internally stored by `std::map`, one for each type.

Each type T participating the rendering
Should provide a function `T *createT(Params&)`

`Loader`: Parse JSON file, convert each field into Params and call corresponding `createT()`.

## 其他

先写每个类的 `createT()`，搞清楚 `Params` 里面要存什么，怎么存；然后写 `Params` 相关的东西。