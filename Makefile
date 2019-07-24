CC=clang
CXX=clang++
RM=rm -f
CPPFLAGS=-g -std=c++17 -Wall -stdlib=libc++ -Iinclude
LDFLAGS=-g -stdlib=libc++ -Iinclude
LDLIBS=
NAME=bc-scanner

SRCS=src/main.cc src/java_class_file.cc src/constant_pool.cc src/constant_pool_entry_parser.cc \
src/field_info.cc src/attribute_info.cc src/method_info.cc src/attribute/code_attribute.cc \
src/attribute/bootstrap_methods_attribute.cc \
src/attribute/annotation_default_attribute.cc src/attribute/constant_value_attribute.cc \
src/attribute/deprecated_attribute.cc src/attribute/enclosing_method_attribute.cc \
src/attribute/exceptions_attribute.cc src/attribute/inner_classes_attribute.cc \
src/attribute/line_number_table_attribute.cc src/attribute/local_variable_table_attribute.cc \
src/attribute/local_variable_type_table_attribute.cc \
src/attribute/runtime_invisible_annotations_attribute.cc \
src/attribute/runtime_invisible_parameter_annotations_attribute.cc \
src/attribute/runtime_visible_annotations_attribute.cc \
src/attribute/runtime_visible_parameter_annotations_attribute.cc \
src/attribute/signature_attribute.cc src/attribute/source_file_attribute.cc \
src/attribute/stack_map_table_attribute.cc src/attribute/synthetic_attribute.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: build

build: $(OBJS)
		$(CXX) $(LDFLAGS) -o $(NAME) $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
		$(RM) ./.depend
		$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJS)

distclean: clean
		$(RM) *~ .depend $(NAME)

include .depend
