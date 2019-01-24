#!/bin/sh

diff models/blog.h ../tf_test01/models/blog.h
diff models/blog.cpp ../tf_test01/models/blog.cpp
diff models/sqlobjects/blogobject.h ../tf_test01/models/sqlobjects/blogobject.h

diff controllers/blogcontroller.cpp ../tf_test01/controllers/blogcontroller.cpp
diff controllers/blogcontroller.h ../tf_test01/controllers/blogcontroller.h

diff views/blog/create.erb ../tf_test01/views/blog/create.erb 
diff views/blog/index.erb ../tf_test01/views/blog/index.erb
diff views/blog/save.erb ../tf_test01/views/blog/save.erb 
diff views/blog/show.erb ../tf_test01/views/blog/show.erb 
