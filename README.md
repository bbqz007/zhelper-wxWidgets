# zhelper-wxWidgets
a helper header help you easy to program gui app on wxWidgets 3.x

you can easily write menus like this
```
Frame* frame = new Frame;
menu::begin(new MenuBar)
    ("File", 
         menu::begin(new Menu)
　　　　　　　　　　　(ID_OPEN, "open")
                        [onclick = [=] (event&) {  }]
                    (ID_NEW,   "new")  
                        [onclick, [=] (event&) {  }]
                    (menu::end)
     )
     ("About",
          menu::begin(new Menu)
　　　　　　　　　　　　　(ID_HELP,   "help")
                            [onclick, [=] (event&)  {   }]
                     (menu::end)
     )
     (menu::end,   
        [=] (MenuBar* mb) {
            frame->SetMenuBar(mb);
        });
```

you can easily write windows like this as well:
```
Frame* frame = new Frame;
layout::begin(new layout)
    (layout::begin(new layout)
        [layout_flags]
        (new textctrl)
            [layout_flags]
            [ ondropfiles = [] (event&) {} ]
            [ onchar = [] (event&) {} ]
        (new button) 
            [layout_flags]
            [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) 
            [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) 
            [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) 
            [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::end, 
     layout::oncomplete = [=] (layout& layout) {
         frame->SetLayout(layout);
     });
```
just easy like writing a html
```
<div id='0'>
    <div id='1'>
        <input type="button" onclick="handler()">
    </div>
    <div id='2'>
        <input type="button" onclick="handler()">
    </div>
    <div id='3'>
        <input type="button" onclick="handler()">
    </div>
    <div id='4'>
        <input type="button" onclick="handler()">
    </div>
</div>
```

how to use?

just include the header and use c++11.

how to write layouts?

1.begin with calling layout::begin(somelayout)

2.add elements(layout or widget) to layout, by calling operator () (widget)

3.configure your latest added elements, by calling operator [] (prop)

4.end with calling operator () (layout::end)

just like 
```
layout::begin(somelayout)   // begin to write a layout
    [prop]              // configure layout properties of somelayout
    (new element)           // add a element to somelayout
        [prop]              // configure layout properties of new element
        [onclick = ...]     // configure onclick function associated with new element
        [onchar = ...]      // configure onchar function associated with new element
    (layout::end)           // end up writing a layout
```
