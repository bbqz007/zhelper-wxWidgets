2020 05/15
new features:
1. easy way to write wxListCtrl headers (columns)
```
wxListCtrl* p = new wxListCtrl;
using namespace zhelper::wxWidgets::listctrlhlp;
column::begin(p)
  ("col 1") [ font ] [ color ]
  ("col 2") [ font ] [ color ]
  (column::end)
```

2. easy way to append rows into wxListCtrl
```
row::begin(p)
  ("row 1_col 1") [ font ] [ color ]
  ("row 1_col 2") [ font ] [ color ]
  (row::another)
  ("row 2_col 1") [ font ] [ color ]
  ("row 2_col 2") [ font ] [ color ]
  (row::end)
```

3. easy way to modify cell of wxListCtrl
```
cell::begin(p)
  (1, 1)    // row 2 , col 2
    [ "change value" ] [ change_font ] [ change_color ]
  (0, 1)    // row 1 , col 2
    [ "change value" ] [ change_font ] [ change_color ]
   (cell:end)
```


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
