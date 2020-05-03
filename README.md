# zhelper-wxWidgets
a helper header help you easy to program gui app on wxWidgets 3.x

you can easily write menus like this
```
Frame* frame = new Frame;
menu::begin(new MenuBar)
    ("File", 
         menu::begin(new Menu)
　　　　　　　　　　　　(ID_OPEN, "open",   [=] (event&) {  })
                    (ID_NEW,   "new",    [=] (event&) {  })
                    (menu::end)
     )
     ("About",
          menu::begin(new Menu)
　　　　　　　　　　　　　(ID_HELP,   "help",    [=] (event&)  {   })
                     (menu::end)
     )
     (menu::end,   [=] (MenuBar* mb) {
                                      frame->SetMenuBar(mb);
                                });
```

you can easily write windows like this as well:
```
Frame* frame = new Frame;
layout::begin(new layout)
    (layout::begin(new layout)
        (new button) [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::begin(new layout)
        (new button) [ onclick = [] (event&) {} ]
        (layout::end)
    )
    (layout::end, [=] (layout& layout) {
                                frame->SetLayout(layout);
                            });
```
