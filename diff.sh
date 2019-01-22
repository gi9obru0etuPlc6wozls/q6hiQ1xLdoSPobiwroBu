#!/bin/sh

diff models/blog.h ../tf_test01/models/blog.h
diff models/blog.cpp ../tf_test01/models/blog.cpp
diff models/sqlobjects/blogobject.h ../tf_test01/models/sqlobjects/blogobject.h

diff controllers/blogcontroller.cpp ../tf_test01/controllers/blogcontroller.cpp
diff controllers/blogcontroller.h ../tf_test01/controllers/blogcontroller.h
