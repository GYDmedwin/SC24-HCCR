PLUGIN_NAME		:= amdgpu_plugin
PLUGIN_SOBJ		:= amdgpu_plugin.so
PLUGIN_MSOBJ    := libmypzstd.so


PLUGIN_INCLUDE  	:= -iquote../../../criu/include
PLUGIN_INCLUDE  	+= -iquote../../criu/include
PLUGIN_INCLUDE  	+= -iquote../../criu/arch/$(ARCH)/include/
PLUGIN_INCLUDE  	+= -iquote../../

PLUGIN_CXXINCLUDE   += -iquote./utils/
PLUGIN_CXXINCLUDE   += -iquote./common/

COMPEL                 := ../../compel/compel-host
LIBDRM_INC 		:= -I/usr/include/libdrm
DEPS_OK 		:= amdgpu_plugin.so amdgpu_plugin_test
DEPS_NOK 		:= ;

include $(__nmk_dir)msg.mk

CC      		:= gcc
PLUGIN_CFLAGS  		:= -g -Wall -Werror -D _GNU_SOURCE -shared -nostartfiles -fPIC
PLUGIN_LDFLAGS		:= -lpthread -lrt -ldrm -ldrm_amdgpu -lmypzstd -L./ -I. -Xlinker -rpath=./ 

CXX             := g++
PLUGIN_CXXFLAGS     := -O3 -Wall -Wextra -pedantic -std=c++11 -shared -fPIC
PLUGIN_CXXLDFLAGS   := -lzstd -lpthread

ifeq ($(CONFIG_AMDGPU),y)
        all: $(DEPS_OK)
else
        all: $(DEPS_NOK)
endif

libmypzstd.so: Pzstd.cpp Options.cpp SkippableFrame.cpp Options_CWrapper.cpp Pzstd_CWrapper.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $^ -o $@ $(PLUGIN_CXXINCLUDE) $(PLUGIN_CXXLDFLAGS)

criu-amdgpu.pb-c.c: criu-amdgpu.proto
		protoc-c --proto_path=. --c_out=. criu-amdgpu.proto

amdgpu_plugin.so: libmypzstd.so amdgpu_plugin.c amdgpu_plugin_topology.c criu-amdgpu.pb-c.c thpool.c
	$(CC) $(PLUGIN_CFLAGS) $(shell $(COMPEL) includes) $^ -o $@ $(PLUGIN_INCLUDE) $(PLUGIN_LDFLAGS) $(LIBDRM_INC)

amdgpu_plugin_clean:
	$(call msg-clean, $@)
	$(Q) $(RM) amdgpu_plugin.so criu-amdgpu.pb-c*
.PHONY: amdgpu_plugin_clean

test_topology_remap: amdgpu_plugin_topology.c tests/test_topology_remap.c
	$(CC) $^ -o $@ -DCOMPILE_TESTS $(PLUGIN_INCLUDE) -I../../include/ -I .

amdgpu_plugin_test:  test_topology_remap
.PHONY: amdgpu_plugin_test

amdgpu_plugin_test_clean:
	$(Q) $(RM) test_topology_remap
.PHONY: amdgpu_plugin_test_clean

clean: amdgpu_plugin_clean amdgpu_plugin_test_clean

mrproper: clean

install:
	$(Q) mkdir -p $(PLUGINDIR)
ifeq ($(CONFIG_AMDGPU),y)
	$(E) "  INSTALL " $(PLUGIN_NAME)
	$(Q) install -m 644 $(PLUGIN_SOBJ) $(PLUGIN_MSOBJ) $(PLUGINDIR)
endif
.PHONY: install

uninstall:
ifeq ($(CONFIG_AMDGPU),y)
	$(E) " UNINSTALL" $(PLUGIN_NAME)
	$(Q) $(RM) $(PLUGINDIR)/$(PLUGIN_SOBJ) 
	$(Q) $(RM) $(PLUGINDIR)/$(PLUGIN_MSOBJ)
endif
.PHONY: uninstall
