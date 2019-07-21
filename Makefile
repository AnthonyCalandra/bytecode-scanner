CC=clang
CXX=clang++
RM=rm -f
CPPFLAGS=-g -std=c++17 -Wall -stdlib=libc++
LDFLAGS=-g -stdlib=libc++
LDLIBS=

SRCS=main.cc java_class_file.cc constant_pool.cc constant_pool_entry_parser.cc \
field_info.cc attribute_info.cc method_info.cc code_attribute.cc bootstrap_methods_attribute.cc \
annotation_default_attribute.cc constant_value_attribute.cc deprecated_attribute.cc \
enclosing_method_attribute.cc exceptions_attribute.cc inner_classes_attribute.cc \
line_number_table_attribute.cc local_variable_table_attribute.cc \
local_variable_type_table_attribute.cc runtime_invisible_annotations_attribute.cc \
runtime_invisible_parameter_annotations_attribute.cc runtime_visible_annotations_attribute.cc \
runtime_visible_parameter_annotations_attribute.cc signature_attribute.cc source_file_attribute.cc \
stack_map_table_attribute.cc synthetic_attribute.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: build

build: $(OBJS)
		$(CXX) $(LDFLAGS) -o bc-scanner $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
		$(RM) ./.depend
		$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJS)

distclean: clean
		$(RM) *~ .depend

include .depend
