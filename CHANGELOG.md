2020 05/14
1. add operator[] to configure layout style properties, which operator can be near to your latest added widget or sizer.
```
  (element)
    [prop]
    [onclick = <void(event&)>]
```
2. you can use \[onclick, <void(event&)>] instead of \[onclick = <void(event&)>]. 
```
  (element1)
     [onclick, <void(event&)>]
  (element2)
     [onclick = <void(event&)>]
```
3. fix bug
```
  operator ()(T1&) [ i = 1 ] operator ()(T2&) [ i = 2 ]
```
the code execution sequence is 

1. i = 2

2. i = 1

3. operator ()(T1&)

4. \[i]

5. operator ()(T2&)

6. \[i]

my aim to 
```
  operator ()(T1&) [ 1 ] operator ()(T2&) [ 2 ]
```
but actually it goes like
```
  operator ()(T1&) [ 1 ] operator ()(T2&) [ 1 ]
```
now the problem is solved.
