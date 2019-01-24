#!/bin/sh

cp models/blog.h ../tf_test01/models/blog.h
cp models/blog.cpp ../tf_test01/models/blog.cpp
cp models/sqlobjects/blogobject.h ../tf_test01/models/sqlobjects/blogobject.h

cp controllers/blogcontroller.cpp ../tf_test01/controllers/blogcontroller.cpp
cp controllers/blogcontroller.h ../tf_test01/controllers/blogcontroller.h

cp views/blog/create.erb ../tf_test01/views/blog/create.erb 
cp views/blog/index.erb ../tf_test01/views/blog/index.erb
cp views/blog/save.erb ../tf_test01/views/blog/save.erb 
cp views/blog/show.erb ../tf_test01/views/blog/show.erb 
