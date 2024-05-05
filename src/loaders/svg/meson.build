ttf_dir = ''
if ttf_loader
   if host_machine.system() == 'linux'
      ttf_dir = '/usr/share/fonts/'
   elif host_machine.system() == 'windows'
      ttf_dir = 'C:\\\\Windows\\\\Fonts\\\\'
   elif host_machine.system() == 'darwin'
      ttf_dir = '/System/Library/Fonts/'
   endif
endif
add_project_arguments('-DTTF_DIR="' + ttf_dir + '"'.format(src_dir), language: 'cpp')

source_file = [
   'tvgSvgCssStyle.h',
   'tvgSvgLoader.h',
   'tvgSvgLoaderCommon.h',
   'tvgSvgPath.h',
   'tvgSvgSceneBuilder.h',
   'tvgSvgUtil.h',
   'tvgXmlParser.h',
   'tvgSvgCssStyle.cpp',
   'tvgSvgLoader.cpp',
   'tvgSvgPath.cpp',
   'tvgSvgSceneBuilder.cpp',
   'tvgSvgUtil.cpp',
   'tvgXmlParser.cpp'
]

subloader_dep += [declare_dependency(
    include_directories : include_directories('.'),
    sources : source_file
)]
