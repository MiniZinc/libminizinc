% 要制造的产品
enum Products;  
% 每种产品的单位利润
array[Products] of int: profit;
% 用到的资源
enum Resources;
% 每种资源可获得的数量
array[Resources] of int: capacity; 

% 制造一个单位的产品需要的资源单位量
array[Products, Resources] of int: consumption; 
constraint assert(forall (r in Resources, p in Products) 
           (consumption[p,r] >= 0), "Error: negative consumption");

% 产品数量的界
int: mproducts = max (p in Products) 
                     (min (r in Resources where consumption[p,r] > 0) 
                          (capacity[r] div consumption[p,r]));

% 变量：每种产品我们需要制造多少
array[Products] of var 0..mproducts: produce;
array[Resources] of var 0..max(capacity): used;

% 产量不可以使用超过可获得的资源量：
constraint forall (r in Resources) (     
      used[r] = sum (p in Products)(consumption[p, r] * produce[p]) 
);
constraint forall (r in Resources) (     
      used[r] <= capacity[r]
);    

% 最大化利润
solve maximize sum (p in Products) (profit[p]*produce[p]);

output [ "\(p) = \(produce[p]);\n" | p in Products ] ++
       [ "\(r) = \(used[r]);\n" | r in Resources ];
