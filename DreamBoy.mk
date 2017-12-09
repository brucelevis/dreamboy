##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=DreamBoy
ConfigurationName      :=Debug
WorkspacePath          :=/home/danny/projects/dreamboy
ProjectPath            :=/home/danny/projects/dreamboy
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Danny
Date                   :=09/12/17
CodeLitePath           :=/home/danny/.codelite
LinkerName             :=/usr/bin/clang++
SharedObjectLinkerName :=/usr/bin/clang++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="DreamBoy.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)SDL2 $(LibrarySwitch)GL 
ArLibs                 :=  "SDL2" "GL" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -Wfatal-errors -g -O0 -std=c++11 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_imgui_imgui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_memory.cpp$(ObjectSuffix) $(IntermediateDirectory)/debugger.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_cpu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_flags.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_bit.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_rom.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_cpuOperations.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	cp src/imgui/extra_fonts/Cousine-Regular.ttf Debug/Cousine-Regular.ttf
	rsync -r roms/ Debug/roms/
	@echo Done

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_imgui_imgui.cpp$(ObjectSuffix): src/imgui/imgui.cpp $(IntermediateDirectory)/src_imgui_imgui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/imgui/imgui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_imgui_imgui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_imgui_imgui.cpp$(DependSuffix): src/imgui/imgui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_imgui_imgui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_imgui_imgui.cpp$(DependSuffix) -MM src/imgui/imgui.cpp

$(IntermediateDirectory)/src_imgui_imgui.cpp$(PreprocessSuffix): src/imgui/imgui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_imgui_imgui.cpp$(PreprocessSuffix) src/imgui/imgui.cpp

$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(ObjectSuffix): src/imgui/imgui_impl_sdl.cpp $(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/imgui/imgui_impl_sdl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(DependSuffix): src/imgui/imgui_impl_sdl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(DependSuffix) -MM src/imgui/imgui_impl_sdl.cpp

$(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(PreprocessSuffix): src/imgui/imgui_impl_sdl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_imgui_imgui_impl_sdl.cpp$(PreprocessSuffix) src/imgui/imgui_impl_sdl.cpp

$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(ObjectSuffix): src/tinyfiledialogs/tinyfiledialogs.cpp $(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/tinyfiledialogs/tinyfiledialogs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(DependSuffix): src/tinyfiledialogs/tinyfiledialogs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(DependSuffix) -MM src/tinyfiledialogs/tinyfiledialogs.cpp

$(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(PreprocessSuffix): src/tinyfiledialogs/tinyfiledialogs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_tinyfiledialogs_tinyfiledialogs.cpp$(PreprocessSuffix) src/tinyfiledialogs/tinyfiledialogs.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(ObjectSuffix): src/imgui/imgui_draw.cpp $(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/imgui/imgui_draw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(DependSuffix): src/imgui/imgui_draw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(DependSuffix) -MM src/imgui/imgui_draw.cpp

$(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(PreprocessSuffix): src/imgui/imgui_draw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_imgui_imgui_draw.cpp$(PreprocessSuffix) src/imgui/imgui_draw.cpp

$(IntermediateDirectory)/src_memory.cpp$(ObjectSuffix): src/memory.cpp $(IntermediateDirectory)/src_memory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/memory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_memory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_memory.cpp$(DependSuffix): src/memory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_memory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_memory.cpp$(DependSuffix) -MM src/memory.cpp

$(IntermediateDirectory)/src_memory.cpp$(PreprocessSuffix): src/memory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_memory.cpp$(PreprocessSuffix) src/memory.cpp

$(IntermediateDirectory)/debugger.cpp$(ObjectSuffix): debugger.cpp $(IntermediateDirectory)/debugger.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/debugger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/debugger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/debugger.cpp$(DependSuffix): debugger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/debugger.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/debugger.cpp$(DependSuffix) -MM debugger.cpp

$(IntermediateDirectory)/debugger.cpp$(PreprocessSuffix): debugger.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/debugger.cpp$(PreprocessSuffix) debugger.cpp

$(IntermediateDirectory)/src_cpu.cpp$(ObjectSuffix): src/cpu.cpp $(IntermediateDirectory)/src_cpu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/cpu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_cpu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_cpu.cpp$(DependSuffix): src/cpu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_cpu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_cpu.cpp$(DependSuffix) -MM src/cpu.cpp

$(IntermediateDirectory)/src_cpu.cpp$(PreprocessSuffix): src/cpu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_cpu.cpp$(PreprocessSuffix) src/cpu.cpp

$(IntermediateDirectory)/src_flags.cpp$(ObjectSuffix): src/flags.cpp $(IntermediateDirectory)/src_flags.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/flags.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_flags.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_flags.cpp$(DependSuffix): src/flags.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_flags.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_flags.cpp$(DependSuffix) -MM src/flags.cpp

$(IntermediateDirectory)/src_flags.cpp$(PreprocessSuffix): src/flags.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_flags.cpp$(PreprocessSuffix) src/flags.cpp

$(IntermediateDirectory)/src_bit.cpp$(ObjectSuffix): src/bit.cpp $(IntermediateDirectory)/src_bit.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/bit.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_bit.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_bit.cpp$(DependSuffix): src/bit.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_bit.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_bit.cpp$(DependSuffix) -MM src/bit.cpp

$(IntermediateDirectory)/src_bit.cpp$(PreprocessSuffix): src/bit.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_bit.cpp$(PreprocessSuffix) src/bit.cpp

$(IntermediateDirectory)/src_rom.cpp$(ObjectSuffix): src/rom.cpp $(IntermediateDirectory)/src_rom.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/rom.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_rom.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_rom.cpp$(DependSuffix): src/rom.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_rom.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_rom.cpp$(DependSuffix) -MM src/rom.cpp

$(IntermediateDirectory)/src_rom.cpp$(PreprocessSuffix): src/rom.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_rom.cpp$(PreprocessSuffix) src/rom.cpp

$(IntermediateDirectory)/src_cpuOperations.cpp$(ObjectSuffix): src/cpuOperations.cpp $(IntermediateDirectory)/src_cpuOperations.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/cpuOperations.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_cpuOperations.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_cpuOperations.cpp$(DependSuffix): src/cpuOperations.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_cpuOperations.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_cpuOperations.cpp$(DependSuffix) -MM src/cpuOperations.cpp

$(IntermediateDirectory)/src_cpuOperations.cpp$(PreprocessSuffix): src/cpuOperations.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_cpuOperations.cpp$(PreprocessSuffix) src/cpuOperations.cpp

$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(ObjectSuffix): src/imgui/imgui_custom_extensions.cpp $(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/danny/projects/dreamboy/src/imgui/imgui_custom_extensions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(DependSuffix): src/imgui/imgui_custom_extensions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(DependSuffix) -MM src/imgui/imgui_custom_extensions.cpp

$(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(PreprocessSuffix): src/imgui/imgui_custom_extensions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_imgui_imgui_custom_extensions.cpp$(PreprocessSuffix) src/imgui/imgui_custom_extensions.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


