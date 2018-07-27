更多复杂模型
===================

在上一节中，我们介绍了MiniZinc模型的基本结构。在这一节中，我们介绍数组和集合数据结构，枚举类型，以及更加复杂的约束。

.. _sec-arrayset:

数组和集合
---------------

在绝大多数情况下，我们都是有兴趣建一个约束和变量的个数依赖于输入数据的模型。
为了达到此目的，我们通常会使用 :index:`数组 <array>` 。 

考虑一个关于金属矩形板温度的简单有限元素模型。通过把矩形板在2维的矩阵上分成有限个的元素，
我们近似计算矩形板上的温度。
一个模型在 :numref:`ex-laplace` 中给出。它声明了有限元素模型的宽 ``w`` 和高 ``h`` 。

声明

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 5-9

声明了四个固定的整型集合来描述有限元素模型的尺寸： ``HEIGHT`` 是整个模型的整体高度，而 ``CHEIGHT`` 是省略了顶部和底部的中心高度， ``WIDTH`` 是模型的整体宽度，而 ``CWIDTH`` 是省略了左侧和右侧的中心宽度。最后，声明了一个浮点型变量组成的行编号从 :math:`0` 到 :math:`w` ，列编号从 :math:`0` 到 :math:`h` 的两维数组 ``t`` 用来表示金属板上每一点的温度。
我们可以用表达式 :mzn:`t[i,j]` 来得到数组中第 :math:`i^{th}` 行和第 :math:`j^{th}` 列的元素。

拉普拉斯方程规定当金属板达到一个稳定状态时，每一个内部点的温度是它的正交相邻点的平均值。约束

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 16-18

保证了每一个内部点 :math:`(i,j)` 是它的四个正交相邻点的平均值。
约束

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 20-24

限制了每一个边的温度必须是相等的，并且给了这些温度名字：``left`` ， ``right`` ， ``top`` 和 ``bottom``。
而约束

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :lines: 26-30

确保了角的温度（这些是不相干的）被设置为0.0。
我们可以用 :numref:`ex-laplace` 中给出的模型来决定一个被分成5 :math:`\times` 5个元素的金属板的温度。其中左右下侧的温度为0，上侧的温度为100。

.. literalinclude:: examples/laplace.mzn
  :language: minizinc
  :name: ex-laplace
  :caption: 决定稳定状态温度的有限元平板模型 (:download:`laplace.mzn <examples/laplace.mzn>`).

运行命令

.. code-block:: bash

  $ minizinc --solver osicbc laplace.mzn

得到输出

.. code-block:: none

    0.00 100.00 100.00 100.00   0.00
    0.00  42.86  52.68  42.86   0.00
    0.00  18.75  25.00  18.75   0.00
    0.00   7.14   9.82   7.14   0.00
    0.00   0.00   0.00   0.00   0.00
  ----------

.. defblock:: 集合

  .. index::
    single: set

  集合变量用以下方式声明

  .. code-block:: minizincdef
    
    set of <类型-实例化> : <变量名> ;

  整型，枚举型（参见后面），浮点型和布尔型集合都可以定义。
  决策变量集合只可以是类型为整型或者枚举型的变量集合。
  集合常量有以下形式

  .. code-block:: minizincdef
  
    { <表达式-1>, ..., <表达式-n> }

  或者是以下形式的整型，枚举型或浮点型 :index:`范围` 表达式

  .. code-block:: minizincdef

    <表达式-1> .. <表达式-2>

  标准的 :index:`集合操作符 <operator; set>` 有：元素属于
  (:mzn:`in`),
  (非严格的) 集合包含 (:mzn:`subset`), 
  (非严格的) 超集关系 (:mzn:`superset`), 并集 
  (:mzn:`union`),
  交集 (:mzn:`intersect`), 
  集合差运算 (:mzn:`diff`), 
  集合对称差 (:mzn:`symdiff`) 
  和集合元素的个数 (:mzn:`card`).

  我们已经看到集合变量和集合常量（包含范围）可以被用来作为变量声明时的隐式类型。
  在这种情况下变量拥有集合元素中的类型并且被隐式地约束为集合中的一个成员。

我们的烤蛋糕问题是一个非常简单的批量生产计划问题例子。在这类问题中，我们希望
去决定每种类型的产品要制造多少来最大化利润。同时制造一个产品会消耗不同数量固定的资源。
我们可以扩展 :numref:`ex-cakes2` 中的MiniZinc模型为一个不限制资源和产品类型的模型去处理这种类型的问题。
这个模型在 :numref:`ex-prod-planning` 中给出。一个（烤蛋糕问题的）数据文件例子在 :numref:`fig-prod-planning-data` 中给出。

.. literalinclude:: examples/simple-prod-planning.mzn
  :language: minizinc
  :name: ex-prod-planning
  :caption: 简单批量生产计划模型 (:download:`simple-prod-planning.mzn <examples/simple-prod-planning.mzn>`).

.. literalinclude:: examples/simple-prod-planning-data.dzn
  :language: minizinc
  :name: fig-prod-planning-data
  :caption: 简单批量生产计划模型的数据文件例子 (:download:`simple-prod-planning-data.dzn <examples/simple-prod-planning-data.dzn>`).

这个模型的新特征是只用枚举类型 :index:`枚举类型 <type; enumerated>` 。
这使得我们可以把资源和产品的选择作为模型的参数。
模型的第一个项

.. code-block:: minizinc

  enum Products;

声明 ``Products`` 为 *未知的* 产品集合。

.. defblock:: 枚举类型

  .. index::
    single: enumerated type
    single enum

  枚举类型，我们称为 ``enums`` , 用以下方式声明

  .. code-block:: minizincdef

    enum <变量名> ;

  一个枚举类型用以下赋值的方式定义

  .. code-block:: minizincdef

    enum <变量名> = { <变量名-1>, ..., <变量名-n> } ;
    
  其中 :mzndef:`<变量名-1>`, ..., :mzndef:`<变量名-n>` 是名为 :mzndef:`<变量名>` 的枚举类型中的元素。
  通过这个定义，枚举类型中的每个元素也被有效地声明为这个类型的一个新的常量。
  声明和定义可以像往常一样结合为一行。

第二个项声明了一个整型数组：

.. code-block:: minizinc

  array[Products] of int: profit;

``profit`` 数组的 :index:`下标集合 <array; index set>` 是 ``Products`` 。理想情况下，这种声明方式表明只有集合 ``Products`` 中的元素才能被用来做数组的下标。

有 :math:`n` 个元素组成的枚举类型中的元素的行为方式和整数 :math:`1\dots n` 的行为方式很像。它们可以被比较，它们可以按照它们出现在枚举类型定义中的顺序被排序，它们可以遍历，它们可以作为数组的下标，实际上，它们可以出现在一个整数可以出现的任何地方。

在数据文件例子中，我们用一列整数来初始化数组

.. code-block:: minizinc

  Products = { BananaCake, ChocolateCake };  
  profit = [400,450];

意思是香蕉蛋糕的利润是400，而巧克力蛋糕的利润是450。
在内部， ``BananaCake`` 会被看成是像整数1一样，而 ``ChocolateCake`` 会被看成像整数2一样。MiniZinc虽然不提供明确的列表类型，但用 :mzn:`1..n` 为下标集合的一维数组表现起来就像列表。我们有时候也会称它们为列表 :index:`lists <list>` 。

根据同样的方法，接下来的两项中我们声明了一个资源集合 ``Resources`` ，
一个表明每种资源可获得量的数组 ``capacity``。

更有趣的是项

.. code-block:: minizinc

  array[Products, Resources] of int: consumption;

声明了一个两维数组 ``consumption`` 。 :mzn:`consumption[p,r]` 的值是制造一单位的产品 :mzn:`p` 所需要的资源 :mzn:`r` 的数量。其中第一个下标是行下标，而第二个下标是列下标。

数据文件包含了一个两维数组的初始化例子:

.. code-block:: minizinc

  consumption= [| 250, 2, 75,  100, 0,
                | 200, 0, 150, 150, 75 |];
            
注意分隔符 ``|`` 是怎样被用来分隔行的。

.. defblock:: 数组

  .. index:
    single: array

  因此，MiniZinc提供一维和多维数组。它们用以下类型来声明：
  
  .. code-block:: minizincdef
  
    array [ <下标集合-1>, ..., <下标集合-n> ] of <类型-实例化>
  
  MiniZinc要求数组声明要给出每一维的下标集合。下标集合或者是一个整型范围，一个
  被初始化为整型范围的集合变量，或者是一个 :index:`枚举类型 <enumerated type>` 。
  数组可以是所有的基类型：整型，枚举型，布尔型，浮点型或者字符串型。
  这些可以是固定的或者不固定的，除了字符串型，它只可以是参数。数组也可以作用于
  集合但是不可以作用于数组。

  :index:`一维数组常量 <array; literal; 1D>` 有以下格式
  
  .. code-block:: minizincdef
  
    [ <表达式-1>, ..., <表达式-n> ]
  
  而 :index:`二维数组常量 <array; literal; 2D>` 有以下格式

  .. code-block:: minizincdef
  
    [| <表达式-1-1>, ..., <表达式-1-n> |
       ...                         |
       <表达式-m-1>, ..., <表达式-m-n> |]

  其中这个数组有 ``m`` 行 ``n`` 列。
  
  内建函数 :mzn:`array1d` ,
  :mzn:`array2d` 等家族可以被用来从一个列表（或者更准确的说是一个一维数组）去实例化任何维度的数组。
  调用

  .. code-block:: minizincdef

    array<n>d(<下标集合-1>, ..., <下标集合-n>, <列表>)

  返回一个 ``n`` 维的数组，它的下标集合在前 ``n`` 个参数给出，最后一个参数包含了数组的元素。
  例如 :mzn:`array2d(1..3, 1..2, [1, 2, 3, 4, 5, 6])` 和
  :mzn:`[|1, 2 |3, 4 |5, 6|]` 是相等的。

  数组元素按照通常的方式获取 :index:`获取 <array; access>` ： :mzn:`a[i,j]` 给出第 :math:`i^{th}` 行第 :math:`j^{th}` 列的元素。

  .. \pjs{New array functions!}

  串联操作符 ``++`` 可以被用来串联两个一维的数组。
  结果得到一个列表，即一个元素从1索引的一维数组。
  例如 :mzn:`[4000, 6] ++ [2000, 500, 500]` 求得 :mzn:`[4000, 6, 2000, 500, 500]` 。内建函数 :mzn:`length` 返回一维数组的长度。

模型的下一项定义了参数 :mzn:`mproducts` 。它被设为可以生产出的任何类型产品的数量上限。
这个确实是一个复杂的内嵌数组推导式和聚合操作符例子。在我们试图理解这些项和剩下的模型之前，我们应该先介绍一下它们。

首先，MiniZinc提供了在很多函数式编程语言都提供的列表推导式。例如，列表推导式 :mzn:`[i + j | i, j in 1..3 where j < i]` 算得 :mzn:`[1 + 2, 1 + 3, 2 + 3]` 等同于 :mzn:`[3, 4, 5]` 。 :mzn:`[3, 4, 5]` 只是一个下标集合为 :mzn:`1..3` 的数组。

MiniZinc同时也提供了集合推导式，它有类似的语法：例如 :mzn:`{i + j | i, j in 1..3 where j < i}` 计算得到集合 :mzn:`{3, 4, 5}` 。

.. defblock:: 列表和集合推导式

  .. index:
    single: comprehension
    single: comprehension; list

  列表推导式的一般格式是
  
  .. code-block:: minizincdef

    [ <表达式> | <生成元表达式> ]
  
  :mzndef:`<表达式>` 指明了如何从 :mzndef:`<生成元表达式>` 产生的元素输出列表中创建元素。
  生成元 :mzndef:`<generator-exp>` 由逗号分开的一列生成元表达式组成，选择性地跟着一个布尔型表达式。
  两种格式是
  
  .. code-block:: minizincdef
  
    <生成元>, ..., <生成元>
    <生成元>, ..., <生成元> where <布尔表达式>
  
  第二种格式中的可选择的 :mzndef:`<布尔型表达式>` 被用作生成元表达式的过滤器：只有满足布尔型表达式的输出列表中的元素才被用来构建元素。 :index:`生成元 <comprehension; generator>` 
  :mzndef:`<generator>`
  有以下格式
  
  .. code-block:: minizincdef
  
    <标识符>, ..., <标识符> in <数组表达式>
  
  每一个标识符是一个 *迭代器* ，轮流从数值表达式中取值，最后一个标识符变化的最迅速。
  
  列表推导式的生成元和 :mzndef:`<布尔型表达式>` 通常不会涉及决策变量。如果它们确实涉及了决策变量，那么产生的列表是一列 :mzndef:`var opt <T>` ，其中$T$是 :mzndef:`<表达式>` 的类型。更多细节，请参考 :ref:`sec-optiontypes` 中有关选项类型 :index:`option types <option type>` 的论述。

  :index:`集合推导式 <comprehension; set>` 几乎和列表推导式一样：唯一的不同是这里使用 ``{`` 和 ``}`` 括住表达式而不是 ``[`` 和 ``]`` 。集合推导式生成的元素必须是固定的 :index:`fixed` ，即不能是决策变量。类似的，集合推导式的生成元和可选择的 :mzndef:`<布尔型表达式>` 必须是固定的。


.. index::
  single: forall

第二，MiniZinc提供了一系列的可以把一维数组的元素聚合起来的内建函数。它们中最有用的可能是 :mzn:``forall`` 。它接收一个布尔型表达式数组（即，约束），返回单个布尔型表达式，它是对数组中的布尔型表达式的逻辑合取。

例如，以下表达式

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

其中 ``a`` 是一个下标集合为 ``1..3`` 的算术数组。它约束了 ``a`` 中的元素是互相不相同的。列表推导式计算得到 :mzn:`[ a[1] != a[2], a[1] != a[3], a[2] != a[3] ]` ，所以 :mzn:`forall` 函数返回逻辑合取 :mzn:`a[1] != a[2] /\ a[1] != a[3] /\ a[2] != a[3]` 。

.. defblock:: 聚合函数
  
  .. index::
    single: aggregation function
    single: sum
    single: product
    single: min
    single: max
    single: forall
    single: exists
    single: xorall
    single: iffall
    single: aggregation function; sum
    single: aggregation function; product
    single: aggregation function; min
    single: aggregation function; max
    single: aggregation function; forall
    single: aggregation function; exists
    single: aggregation function; xorall
    single: aggregation function; iffall

  算术数组的 *聚合函数* 有: :mzn:`sum` 把元素加起来， :mzn:`product` 把元素乘起来，和 :mzn:`min` 跟 :mzn:`max` 各自返回数组中的最小和最大元素。
  当作用于一个空的数组时， :mzn:`min` 和 :mzn:`max` 返回一个运行错误， :mzn:`sum` 返回0， :mzn:`product` 返回1。

  MiniZinc为数组提供了包含有布尔型表达式的四个聚合函数。
  正如我们看到的，它们中的第一个是 :mzn:`forall` ，它返回一个等于多个约束的逻辑合取的单个约束。
  第二个函数， :mzn:`exists` ，返回多个约束的逻辑析取。因此 :mzn:`forall` 强制数组中的所有约束都满足，而 :mzn:`exists` 确保至少有一个约束满足。第三个函数， :mzn:`xorall` 确保奇数个约束满足。第四个函数， :mzn:`iffall` 确保偶数个约束满足。

第三个，也是难点的最后一个部分是当使用数组推导式时，MiniZinc允许使用一个特别的聚合函数的语法。
建模者不仅仅可以用

.. code-block:: minizinc

  forall( [a[i] != a[j] | i,j in 1..3 where i < j])

也可以用一个更加数学的表示

.. code-block:: minizinc

  forall (i,j in 1..3 where i < j) (a[i] != a[j])

两种表达方式是完全相等的：建模者可以自由使用任何一个他们认为更自然的表达方式。

.. defblock:: 生成表达式

  .. index::
    single: generator call
    single: expression; generator call

  一个 *生成表达式* 有以下格式

  .. code-block:: minizincdef
  
    <聚合函数> ( <生成元表达式> ) ( <表达式> )
  
  圆括号内的 :mzndef:`<生成元表达式>` 以及构造表达式 :mzndef:`<表达式>` 是非选择性的：它们必须存在。它等同于

  .. code-block:: minizincdef
  
    <聚合函数> ( [ <表达式> | <生成元表达式> ] )

  :mzndef:`<聚合函数>` :index:`aggregation function` 可以是MiniZinc的任何由单个数组作为其参数的函数。 

接下来我们就来了解 :numref:`ex-prod-planning` 中的简单批量生产计划模型剩余的部分。现在请暂时忽略定义 :mzn:`mproducts` 的这部分。接下来的项：

.. code-block:: minizinc

  array[Products] of var 0..mproducts: produce;

定义了一个一维的决策变量数组 :mzn:`produce` 。 :mzn:`produce[p]` 的值代表了最优解中产品 :mzn:`p` 的数量。
下一项

.. code-block:: minizinc

  array[Resources] of var 0..max(capacity): used;

定义了一个辅助变量集合来记录每一种资源的使用量。
下面的两个约束

.. code-block:: minizinc

  constraint forall (r in Resources)      
             (used[r] = sum (p in Products) (consumption[p, r] * produce[p]));
  constraint forall (r in Resources)(used[r] <= capacity[r] );

使用 :mzn:`used[r]` 计算资源 :mzn:`r` 的总体消耗以及保证它是少于可获得的资源 :mzn:`r` 的量。
最后，项

.. code-block:: minizinc

  solve maximize sum (p in Products) (profit[p]*produce[p]);

表明这是一个最大化问题以及最大化的目标是全部利润。

现在我们回到 :mzn:`mproducts` 的定义。对每个产品 :mzn:`p` ，表达式

.. code-block:: minizinc

  (min (r in Resources where consumption[p,r] > 0) 
                                   (capacity[r] div consumption[p,r])

决定了在考虑了每种资源 :mzn:`r` 的数量以及制造产品 :mzn:`p` 需要的 :mzn:`r` 量的情况下， :mzn:`p` 可以生产的最大量。注意过滤器 :mzn:`where consumption[p,r] > 0` 的使用保证了只有此产品需要的资源才会考虑，因此避免了出现除数为零的错误。所以，完整的表达式

.. code-block:: minizinc

  int: mproducts = max (p in Products) 
                       (min (r in Resources where consumption[p,r] > 0) 
                                   (capacity[r] div consumption[p,r]));

计算 *任何* 产品可以被制造的最大量，因此它可以被作为 :mzn:`produce` 中的决策变量定义域的上限。

最后，注意输出项是比较复杂的，并且使用了列表推导式 :index:`列表推导式 <comprehension; list>` 去创建一个易于理解的输出。运行

.. code-block:: bash

  $ minizinc --solver gecode simple-prod-planning.mzn simple-prod-planning-data.dzn

输出得到如下结果

.. code-block:: none

  BananaCake = 2;
  ChocolateCake = 2;
  Flour = 900;
  Banana = 4;
  Sugar = 450;
  Butter = 500;
  Cocoa = 150;
  ----------
  ==========


全局约束
------------------

.. \index{constraint!global|see{global constraint}}
.. \index{global constraint}

MiniZinc包含了一个全局约束的库，这些全局约束也可以被用来定义模型。一个例子是 :mzn:`alldifferent` 约束，它要求所有参数中的变量都必须是互相不相等的。

.. literalinclude:: examples/send-more-money.mzn
  :language: minizinc
  :name: ex-smm
  :caption: SEND+MORE=MONEY算式谜题模型 (:download:`send-more-money.mzn <examples/send-more-money.mzn>`)

SEND+MORE=MONEY问题要求给每一个字母赋不同的数值使得此算术约束满足。 :numref:`ex-smm` 中的模型使用 :mzn:`alldifferent([S,E,N,D,M,O,R,Y])` 约束表达式来保证每个字母有不同的数字值。在使用了引用项 

.. code-block:: minizinc

  include "alldifferent.mzn";

后，此全局约束 :mzn:`alldifferent` 可以在模型中使用。
我们可以用以下代替此行

.. code-block:: minizinc

  include "globals.mzn";

它包含了所有的全局约束。

一系列所有在MiniZinc中定义了的全局约束都被包含在了发布的文档中。对一些重要的全局约束的描述，请参见 :ref:`sec-globals` 。

条件表达式
-----------------------

.. \index{expression!conditional}

MiniZinc提供了一个条件表达式 *if-then-else-endif* 。
它的一个使用例子如下

.. code-block:: minizinc

  int: r = if y != 0 then x div y else 0 endif;

若 :mzn:`y` 不是零，则 :mzn:`r` 设为 :mzn:`x` 除以 :mzn:`y` ，否则则设为零。

.. defblock:: 条件表达式

  .. index::
    single: expression; conditional

  条件表达式的格式是

  .. code-block:: minizincdef

    if <布尔型表达式> then <表达式-1> else <表达式-2> endif

  它是一个真表达式而不是一个控制流语句，所以它可以被用于其他表达式中。如果 :mzndef:`<布尔型表达式>` 是真，则它取值 :mzndef:`<表达式-1>` ，否则则是 :mzndef:`<表达式-2>` 。 条件表达式的类型是 :mzndef:`<表达式-1>` 和 :mzndef:`<表达式-2>` 的类型，而它们俩必须有相同的类型。

  如果 :mzndef:`<布尔型表达式>` 包含决策变量，则表达式的类型-实例化是 :mzndef:`var <T>` ，其中 :mzndef:`<T>` 是
  :mzndef:`<表达式-1>` 和 :mzndef:`<表达式-2>` 的类型，就算是在两个表达式都已经固定了的情况下也是如此。

.. literalinclude:: examples/sudoku.mzn
  :language: minizinc
  :name: ex-sudoku
  :caption: 广义数独问题的模型 (:download:`sudoku.mzn <examples/sudoku.mzn>`)


.. literalinclude:: examples/sudoku.dzn
  :language: minizinc
  :name: ex-sudokud
  :caption: 广义数独问题的数据文件例子 (:download:`sudoku.dzn <examples/sudoku.dzn>`)

.. _fig-sudoku:

.. figure:: figures/sudoku.*
  
  :download:`sudoku.dzn <examples/sudoku.dzn>` 代表的问题。

在创建复杂模型或者复杂输出时，条件表达式是非常有用的。我们来看下 :numref:`ex-sudoku` 中的数独问题模型。板的初始位置在参数 :mzn:`start` 中给出，其中0代表了一个空的板位置。通过使用以下条件表达式

.. code-block:: minizinc

  constraint forall(i,j in PuzzleRange)(
       if start[i,j] > 0 then puzzle[i,j] = start[i,j] else true endif );

它被转换为对决策变量 :mzn:`puzzle` 的约束。

在定义复杂输出时，条件表达式也很有用。 :index:`output` 在数独模型 :numref:`ex-sudoku` 中，表达式

.. code-block:: minizinc

  if j mod S == 0 then " " else "" endif 

在大小为 :mzn:`S` 的组群之间插入了一个额外的空格。输出表达式同时也使用条件表达式来在每 :mzn:`S` 行后面加入一个空白行。这样得到的输出有很高的可读性。

剩下的约束保证了每行中，每列中以及每 :math:`S \times S` 子方格块中的值都是互相不相同的。

.. index::
  single: runtime flag; -a
  single: runtime flag; --all-solutions
  single: solution; all

通过使用标示 ``-a`` 或 ``--all-solutions`` ，我们可以用MiniZinc求解得到一个满足问题 (:mzn:`solve satisfy`) 的所有解。
运行

.. code-block:: bash

  $ minizinc --all-solutions sudoku.mzn sudoku.dzn

得到

.. code-block:: none

   5 9 3  7 6 2  8 1 4 
   2 6 8  4 3 1  5 7 9 
   7 1 4  9 8 5  2 3 6 

   3 2 6  8 5 9  1 4 7
   1 8 7  3 2 4  9 6 5
   4 5 9  1 7 6  3 2 8

   9 4 2  6 1 8  7 5 3
   8 3 5  2 4 7  6 9 1
   6 7 1  5 9 3  4 8 2
  ----------
  ==========

当系统输出完所有可能的解之后，此行 ``==========`` 被输出。在这里则表明了此问题只有一个解。


.. _sec-enum:

枚举类型
----------------

.. index::
  single: type; enumerated

枚举类型允许我们根据一个或者是数据中的一部分，或者在模型中被命名的对象集合来创建模型。这样一来，模型就更容易被理解和调试。我们之前已经简单介绍了枚举类型或者enums。在这一小分段，我们会探索如何可以全面地使用它们，并且给出一些处理枚举类型的内建函数。

让我们重新回顾一下 :ref:`sec-modelling` 中的给澳大利亚涂色问题。

.. literalinclude:: examples/aust-enum.mzn
  :language: minizinc
  :name: ex-aust-enum
  :caption: 使用枚举类型的澳大利亚涂色模型 (:download:`aust-enum.mzn <examples/aust-enum.mzn>`).

:numref:`ex-aust-enum` 中的模型声明了一个枚举类型 :mzn:`Color` ，而它必须在数据文件中被定义。每一个州变量被声明为从此枚举类型中取一个值。使用以下方式运行这个程序 

.. code-block:: bash
  
  $ minizinc -D"Color = { red, yellow, blue };" aust-enum.mzn

可能会得到输出

.. code-block:: none

  wa = yellow;
  nt = blue;
  sa = red;
  q = yellow;
  nsw = blue;
  v = yellow;
  t = red;


.. defblock:: 枚举类型变量声明

  .. index::
    single: variable; declaration; enum

  一个枚举类型参数变量被声明为以下两种方式： 
  
  .. code-block:: minizincdef
  
    <枚举名> : <变量名>
    <l>..<u> : <变量名>
  
  其中 :mzndef:`<枚举名>` 是枚举类型的名字， :mzndef:`<l>` 和 :mzndef:`<u>` 是此枚举类型的固定枚举类型表达式。

枚举类型一个重要的行为是，当它们出现的位置所期望的是整数时，它们会自动地强制转换为整数。这样一来，这就允许我们使用定义在整数上的全局变量，例如

.. code-block:: minizinc

  global_cardinality_low_up([wa,nt,sa,q,nsw,v,t],
                            [red,yellow,blue],[2,2,2],[2,2,3]);

要求每种颜色至少有两个州涂上并且有三个州被涂了蓝色。


.. defblock:: 枚举类型操作符

  有一系列关于枚举类型的内部操作符：

  - :mzn:`enum_next(X,x)`: 返回枚举类型 :mzn:`X` 中 :mzn:`x` 后的下一个值。 这是一个部份函数, 如果 :mzn:`x` 是枚举类型 :mzn:`X` 最后一个值, 则函数会返回 :math:`\bot` 令包含这个表达式的布尔表达式返回 :mzn:`false` 。
  - :mzn:`enum_prev(X,x)` :mzn:`enum_prev(X,x)`: 返回枚举类型 :mzn:`X` 中 :mzn:`x` 的上一个值。 :mzn:`enum_prev` 同样是一个部份函数。
  - :mzn:`to_enum(X,i)`: 映射一个整型表达式 :mzn:`i` 到一个在 :mzn:`X` 的枚举类型值, 或者如果 :mzn:`i` 是小于等于0或大于:mzn:`X`中元素的个数, 则返回 :math:`\bot` 。

  注意，一些标准函数也是可以应用于枚举类型上 

  - :mzn:`card(X)`: 返回枚举类型 :mzn:`X` 的势。
  - :mzn:`min(X)`: 返回枚举类型 :mzn:`X` 中最小的元素。
  - :mzn:`max(X)`: 返回枚举类型 :mzn:`X` 中最大的元素。 


.. _sec-complex:

复杂约束
-------------------

.. index::
  single: constraint; complex

约束是MiniZinc模型的核心。我们已经看到了简单关系表达式，但是约束其实是比这更加强大的。
一个约束可以是任何布尔型表达式。想象一个包含两个时间上不能重叠的任务的调度问题。如果 :mzn:`s1` 和 :mzn:`s2` 是相对应的起始时间， :mzn:`d1` 和 :mzn:`d2` 是相对应的持续时间，我们可以表达约束为： 

.. code-block:: minizinc

  constraint s1 + d1 <= s2  \/ s2 + d2 <= s1;

来保证任务之间互相不会重叠。


.. defblock:: 布尔型

  .. index::
    single: Boolean
    single: expression; Boolean
    single: true
    single: false
    single: operator; Boolean
    single: bool2int

  MiniZinc中的布尔型表达式可以按照标准的数学语法来书写。布尔常量是 :mzn:`真`  或 :mzn:`假` ，布尔型操作符有合取，即，与 (``/\``) ，析取，即，或 (``\/``) ，必要条件蕴含 (:mzn:`<-`) ，充分条件蕴含 (:mzn:`->`) ，充分必要蕴含 (:mzn:`<->`) 以及非 (:mzn:`not`)。内建函数 :mzn:`bool2int` 强制转换布尔型为整型：如果参数为真，它返回1，否则返回0。

.. literalinclude:: examples/jobshop.mzn
  :language: minizinc
  :name: ex-jobshop
  :caption: 车间作业调度问题模型 (:download:`jobshop.mzn <examples/jobshop.mzn>`).

.. literalinclude:: examples/jdata.dzn
  :language: minizinc
  :name: ex-jdata
  :caption: 车间作业调度问题数据 (:download:`jdata.dzn <examples/jdata.dzn>`).

:numref:`ex-jobshop` 中的车间作业调度模型给出了一个使用析取建模功能的现实例子。车间作业调度问题中，我们有一个作业集合，每一个包含一系列的在不同机器上的任务：任务 :mzn:`[i,j]` 是在第 :math:`i^{th}` 个作业中运行在第 :math:`j^{th}` 个机器上的任务。每列任务必须按照顺序完成，并且运行在同一个机器上的任何两个任务在时间上都不能重叠。就算是对这个问题的小的实例找最优解都会是很有挑战性的。

命令

.. code-block:: bash
  
  $ minizinc --all-solutions jobshop.mzn jdata.dzn

求解了一个小的车间作业调度问题，并且显示了优化问题在 :mzn:`all-solutions` 下的表现。在这里，求解器只有当找到一个更好的解时才会输出它，而不是输出所有的可能最优解。这个命令下的（部分）输出是：

.. code-block:: none

  end = 39
   5  9 13 22 30 
   6 13 18 25 36 
   0  4  8 12 16 
   4  8 12 16 22 
   9 16 25 27 38 
  ----------
  end = 37
   4  8 12 17 20 
   5 13 18 26 34 
   0  4  8 12 16 
   8 12 17 20 26 
   9 16 25 27 36 
  ----------
  end = 34
   0  1  5 10 13 
   6 10 15 23 31 
   2  6 11 19 27 
   1  5 10 13 19 
   9 16 22 24 33 
  ----------
  end = 30
   5  9 13 18 21 
   6 13 18 25 27 
   1  5  9 13 17 
   0  1  2  3  9 
   9 16 25 27 29 
  ----------
  ==========

表明一个结束时间为30的最优解终于被找到，并且被证明为是最优的。
通过加一个约束 :mzn:`end = 30` ，并且把求解项改为 :mzn:`solve satisfy` ，然后运行

.. code-block:: bash

  $ minizinc --all-solutions jobshop.mzn jobshop.dzn

我们可以得到所有的 *最优解* 。
这个问题有3,444,375个最优解。

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :name: ex-stable-marriage
  :caption: 稳定婚姻问题模型 (:download:`stable-marriage.mzn <examples/stable-marriage.mzn>`).

.. literalinclude:: examples/stable-marriage.dzn
  :language: minizinc
  :name: ex-sm-data
  :caption: 稳定婚姻问题模型的数据文件例子。 (:download:`stable-marriage.dzn <examples/stable-marriage.dzn>`).

MiniZinc中的另外一个强大的建模特征是决策变量可以被用来访问数组 :index:`array access <array; access>` 。作为一个例子，考虑（老式的） *稳定婚姻问题*  。我们有 :mzn:`n` 个（直）女以及 :mzn:`n` 个（直）男。每一个男士有一个女士排行榜，女士也是。我们想给每一个女士/男士找一个丈夫/妻子来使得所有的婚姻按以下意义上来说都是 *稳定的* ：

- 每当 :mzn:`m` 喜欢另外一个女士 :mzn:`o` 多过他的妻子 :mzn:`w` 时， :mzn:`o` 喜欢她的丈夫多过 :mzn:`m` ，以及 
- 每当 :mzn:`w` 喜欢另外一个男士 :mzn:`o` 多过她的丈夫 :mzn:`m` 时， :mzn:`o` 喜欢他的妻子多过 :mzn:`w`。 

这个问题可以很优雅地在MiniZinc中建模。模型和数据例子在 :numref:`ex-stable-marriage` 和 :numref:`ex-sm-data` 中分别被给出。

模型中的前三项声明了男士/女士的数量以及男士和女士的集合。在这里我们介绍 *匿名枚举类型* 的使用。 :mzn:`Men` 和 :mzn:`Women` 都是大小为 :mzn:`n` 的集合，但是我们不希望把它们混合到一起，所以我们使用了一个匿名枚举类型。这就允许MiniZinc检测到使用 :mzn:`Men` 为 :mzn:`Women` 或者反之的建模错误。

矩阵 :mzn:`rankWomen` 和 :mzn:`rankMen` 分别给出了男士们的女士排行以及女士们的男士排行。因此，项 :mzn:`rankWomen[w,m]` 给出了女士
\texttt{w}的关于男士\texttt{m}的排行。在排行中的数目越小，此男士或者女士被选择的倾向越大。

有两个决策变量的数组： :mzn:`wife` 和 :mzn:`husband` 。这两个分别代表了每个男士的妻子和每个女士的丈夫。

前两个约束

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :lines: 13-14
  
确保了丈夫和妻子的分配是一致的： :mzn:`w` 是 :mzn:`m` 的妻子蕴含了 :mzn:`m` 是 :mzn:`w` 的丈夫，反之亦然。注意在 :mzn:`husband[wife[m]]` 中，下标表达式 :mzn:`wife[m]` 是一个决策变量，而不是一个参数。 

接下来的两个约束是稳定条件的直接编码：

.. literalinclude:: examples/stable-marriage.mzn
  :language: minizinc
  :lines: 16-22

在有了用决策变量作为数组的下标和用标准的布尔型连接符构建约束的功能后，稳定婚姻问题的自然建模才变得可行。敏锐的读者可能会在这时产生疑问，如果数组下标变量取了一个超出数组下标集合的值，会产生什么情况。MiniZinc把这种情况看做失败：一个数组访问 :mzn:`a[e]` 在其周围最近的布尔型语境中隐含地加入了约束 :mzn:`e in index_set(a)` ，其中 :mzn:`index_set(a)` 给出了 :mzn:`a` 的下标集合。

.. defblock:: 匿名枚举类型

  .. index::
    single: type; enumerated; anonymous
  
  一个 *匿名枚举类型* 表达式有格式 :mzndef:`anon_enum(<n>)` ，其中 :mzndef:`<n>` 是一个固定的整型表达式，它定义了枚举类型的大小。

  除了其中的元素没有名字，匿名枚举类型和其他的枚举类型一样。当被输出时，它们根据枚举类型的名字被给定独有的名字。

例如，如下的变量声明

.. code-block:: minizinc

  array[1..2] of int: a= [2,3];
  var 0..2: x;
  var 2..3: y;

约束 :mzn:`a[x] = y` 会在 :math:`x=1 \wedge y=2` 和 :math:`x=2 \wedge y=3` 时得到满足。约束 :mzn:`not a[x] = y` 会在 :math:`x=0 \wedge y=2`，:math:`x=0 \wedge y=3` , :math:`x=1 \wedge y=3` 和 :math:`x=2 \wedge y=2` 时得到满足。

当参数无效访问数组时，正式的MiniZinc语义会把此情况看成失败来确保参数和决策变量
的处理方式是一致的，但是会发出警告，因为这种情况下几乎总是会有错误出现。

.. literalinclude:: examples/magic-series.mzn
  :language: minizinc
  :name: ex-magic-series
  :caption: 魔术串问题模型 (:download:`magic-series.mzn <examples/magic-series.mzn>`).

.. index::
  single: bool2int
  single: constraint; higher order

强制转换函数 :mzn:`bool2int` 可以被任何布尔型表达式调用。
这就使得MiniZinc建模者可以使用所谓的 *高价约束* 。
举个简单的例子，请看 *魔术串问题* ：找到一列数字 :math:`s= [s_0,\ldots,s_{n-1}]` 使得:math:`s_i` 是数字 :math:`i` 出现在 :math:`s` 的次数。一个解的例子是 :math:`s = [1,2,1,0]` 。

这个问题的一个MiniZinc模型在 :numref:`ex-magic-series` 中给出。 :numref:`ex-magic-series` 的使用
使得我们可以把函数 :mzn:`s[j]=i` 满足的次数加起来。
运行命令

.. code-block:: bash

  $ minizinc --all-solutions magic-series.mzn -D "n=4;"

得到输出

.. code-block:: none

  s = [1, 2, 1, 0];
  ----------
  s = [2, 0, 2, 0];
  ----------
  ==========

确切地显示出这个问题的两个解。

注意当有需要的时候，MiniZinc会自动地强制转换布尔型为整型以及整型为浮点型。
我们可以把 :numref:`ex-magic-series` 中的约束项替换为

.. code-block:: minizinc

  constraint forall(i in 0..n-1) (
     s[i] = (sum(j in 0..n-1)(s[j]=i)));

由于MiniZinc系统实际上会自动地加入缺失的 :mzn:`bool2int`，布尔型表达式 
:mzn:`s[j] = i` 会被自动地强制转换为整型，所以会得到同样的结果。

.. defblock:: 强制转换

  .. index::
    single: coercion; automatic
    single: coercion; bool2int
    single: coercion; int2float

  MiniZinc中，通过使用函数 :mzn:`bool2int` ，我们可以把一个布尔型数值\emph{强制转换}为一个整型数值。
  同样地，通过使用函数 :mzn:`int2float` ，我们也可以把一个整型数值强制转换为一个浮点型数值。
  被强制转换的数值的实例化和原数值一样。例如，:mzn:`par bool` 被强制转换为 :mzn:`par int` ，而 :mzn:`var bool` 被强制转换为 :mzn:`var int` 。

  通过适当地在模型中加入 :mzn:`bool2int` 和 :mzn:`int2float` ，MiniZinc会自动地强制转换布尔型表达式为整型表达式，以及整型表达式为浮点型表达式。注意通过两步转换，它也会强制转换布尔型为浮点型。

集合约束
---------------

.. index::
  single: constraint; set

MiniZinc另外一个强大的建模特征是它允许包含整数的集合是决策变量：这表示当模型被评估时，求解器会查找哪些元素在集合中。

举个简单的例子， *0/1背包问题* 。这个问题是背包问题的局限版本，即我们或者选择把物品放入背包或者不放。每一个物品有一个重量和一个利润，在受限制于背包不能太满的条件下，我们想找到选取哪些物品会得到最大化利润。

很自然地，我们在MiniZinc中使用单个的决策变量来建模： :mzn:`var set of ITEM: knapsack` 其中 :mzn:`ITEM` 是可放置的物品集合。如果数组 :mzn:`weight[i]` 和 :mzn:`profit[i]` 分别
给出物品 :mzn:`i` 的重量和利润，以及背包可以装载的最大重量是 :mzn:`capacity`，则一个自然的模型在 :numref:`ex-knapsack-binary` 中给出。

.. literalinclude:: examples/knapsack.mzn
  :language: minizinc
  :name: ex-knapsack-binary
  :caption: 0/1背包问题模型 (:download:`knapsack.mzn <examples/knapsack.mzn>`).

注意，关键字 :mzn:`var` 出现在 :mzn:`set` 声明之前，表明这个集合本身是决策变量。这就和一个 :mzn:`var` 关键字描述其中元素而不是数组自身的数组形成对比，因为此时数组的基本结构，即它的下标集合，是固定了的。

.. literalinclude:: examples/social-golfers.mzn
  :language: minizinc
  :name: ex-social-golfers
  :caption: 高尔夫联谊问题模型 (:download:`social-golfers.mzn <examples/social-golfers.mzn>`).

我们来看一个更复杂的关于集合约束的例子， :numref:`ex-social-golfers` 中给出的高尔夫联谊问题。
这个问题的目的是给 :mzn:`groups` :math:`\times` :mzn:`size` 个高尔夫手在 :mzn:`weeks` 时间内安排一个高尔夫联赛。每一周我们需要安排 :mzn:`groups` 个大小为 :mzn:`size` 的不同的组。任何一对高尔夫手都不能一起出现于两个组中进行比赛。

模型中的变量是第 :math:`i^{th}` 周第 :mzn:`j^{th}` 组的高尔夫手:mzn:`Sched[i,j]` 组成的集合。

11-32行中的约束首先对每一周的第一个集合进行一个排序来去除掉周之间可以互相调换的对称。
然后它对每一周内的集合进行了一个排序，同时使得每一个集合的势为 :mzn:`size` 。
接下来通过使用全局约束 :mzn:`partition_set` ，确保了每一周都是对高尔夫手集合的一个划分。
最后一个约束确保了任何两个高尔夫手都不会一起在两个组内比赛（因为任何两个组的交集的势最多都是1）。

.. index::
  single: symmetry; breaking

我们也有

在34-46行中，我们也给出了去对称初始化约束：
第一周被固定为所有的高尔夫手都按顺序排列；第二周的第一组被规定为是由第一周的前几组的第一个选手组成；最后，对于剩下的周，模型规定第一个 :mzn:`size` 内的高尔夫手们出现在他们相对应的组数中。 

运行命令

.. code-block:: bash

  $ minizinc social-golfers.mzn social-golfers.dzn

其中数据文件定义了一个周数为4，大小为3，组数为4的问题，得到如下结果

.. code-block:: none

  1..3 4..6 7..9 10..12 
  { 1, 4, 7 } { 2, 5, 10 } { 3, 9, 11 } { 6, 8, 12 }
  { 1, 5, 8 } { 2, 6, 11 } { 3, 7, 12 } { 4, 9, 10 }
  { 1, 6, 9 } { 2, 4, 12 } { 3, 8, 10 } { 5, 7, 11 }
  ----------

注意范围集合是如何以范围格式输出的。

汇总
-----------------------

我们以一个可以阐释这一章介绍的大部分特征的复杂例子来结束这一节，
包括枚举类型，复杂约束，全局约束以及复杂输出。

.. literalinclude:: examples/wedding.mzn
  :language: minizinc
  :name: ex-wedding
  :caption: 使用枚举类型规划婚礼座位 (:download:`wedding.mzn <examples/wedding.mzn>`).

:numref:`ex-wedding` 中的模型安排婚礼桌上的座位。这个桌子有12个编码的顺序排列的座位，每边有6个。
男士必须坐奇数号码的座位，女士坐偶数。Ed由于恐惧症不能坐在桌子的边缘，新郎和新娘必须坐在彼此旁边。我们的目的是最大化已知的互相憎恶的人之间的距离。如果在同一边，座位之间的距离是座位号码之间的差，否则则是和其对面座位的距离+ 1。

注意在输出语句中我们观察每个座位 :mzn:`s` 来找一个客人 :mzn:`g` 分配给此座位。我们利用内建函数 :mzn:`fix` ，它检查一个决策变量是否是固定的以及输出它的固定值，否则的话中断。在输出语句中使用此函数总是安全的，因为当输出语句被运行的时候，所有的决策变量都应该是固定了的。

运行

.. code-block:: bash

  $ minizinc wedding.mzn

得到输出

.. code-block:: none

  ted bride groom rona ed carol ron alice bob bridesmaid bestman clara 
  ----------
  ==========

最终得到的座位安排在 :numref:`fig-wedding` 中给出。其中连线表示互相憎恶，总的距离是22.


.. _fig-wedding:

.. figure:: figures/wedding.*
  
  婚礼桌上座位的安排


.. \pjs{Move the fix definition elsewhere!}

.. defblock:: 固定

  .. index::
    single: fix
    single: fixed

  输出项中，内建函数 :mzn:`fix` 检查一个决策变量的值是否固定，然后把决策变量的实例化强制转换为参数。

.. % oil-blending
.. %arrays floats sum forall
.. %more complex datafile
.. 
.. %suduko
.. %2-D array
.. %complex transformation from data file
.. 
.. %jobshop
.. %disjunction,
.. 
.. %talk about other complex constraints--IC example?
.. 
.. %magic sequence
.. %reification
.. 
.. %warehouse placement
.. %reification more complex example
.. 
.. %0/1 knapsack
.. %set constraint
.. 
.. %social golfers
.. %more complex set constraint
.. 
.. %finish with larger example from Mark

