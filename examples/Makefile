ifndef PVFMM_DIR
$(error Cannot find file: PVFMM_DIR)
endif

ifndef TBSLAS_DIR
$(error Cannot find file: TBSLAS_DIR)
endif

-include $(PVFMM_DIR)/MakeVariables

ifndef CXXFLAGS_PVFMM
$(error Cannot find file: MakeVariables)
endif

TBSLAS_INC = -I$(TBSLAS_DIR)/src
CXXFLAGS_TBSLAS = -std=c++0x

RM = rm -f
MKDIRS = mkdir -p

BINDIR = ./bin
SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./include

TARGET_BIN = \
       $(BINDIR)/semilag_tree \
       $(BINDIR)/zalesak \
       $(BINDIR)/diffusion \
       $(BINDIR)/advect-diff 


all : $(TARGET_BIN)

ifeq ($(INTEL_OFFLOAD_OK),yes)

$(BINDIR)/%: $(OBJDIR)/%.o
	-@$(MKDIRS) $(dir $@)
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS) -no-offload         $^       $(LDFLAGS_PVFMM) -o $@
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS)                     $^_async $(LDFLAGS_PVFMM) -o $@_async
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS) -D__DEVICE_SYNC__=1 $^_mic   $(LDFLAGS_PVFMM) -o $@_mic

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-@$(MKDIRS) $(dir $@)
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS) -no-offload         -I$(INCDIR) $(TBSLAS_INC) -c $^ -o $@
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS)                     -I$(INCDIR) $(TBSLAS_INC) -c $^ -o $@_async
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS) -D__DEVICE_SYNC__=1 -I$(INCDIR) $(TBSLAS_INC) -c $^ -o $@_mic

else

$(BINDIR)/%: $(OBJDIR)/%.o
	-@$(MKDIRS) $(dir $@)
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS)                   $^       $(LDFLAGS_PVFMM) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-@$(MKDIRS) $(dir $@)
	$(CXX_PVFMM) $(CXXFLAGS_PVFMM) $(CXXFLAGS_TBSLAS)                 -I$(INCDIR) $(TBSLAS_INC) -c $^ -o $@

endif

clean:
	$(RM) -r $(BINDIR)/* $(OBJDIR)/*
	$(RM) *~ */*~