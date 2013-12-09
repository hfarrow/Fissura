" Initialization {{{
    let g:fs_last_config = "Debug"
    let g:fs_last_project = "all"

    let g:fs_debug_terminal_active = 1

    function! InitFSBuildMenu()

        compiler clang

        if !exists('g:FS_IGNORED_CONFIG_NAMES')
            let g:FS_IGNORED_CONFIG_NAMES = []
        endif

        call GenerateMenus()
    endfunction
" }}}

" Convert paths to the last directory name {{{
    function! ConvertPathToName(paths, out_list)
        for path in a:paths
            let name = split(path, '/')[-1]
            if name !~ 'CMakeLists.txt'
                call add(a:out_list, name)
            endif
        endfor
    endfunction

    function! InitNameLists()
        let g:fs_config_names = []
        let g:fs_module_names = []
        let g:fs_module_spike_names = {}
        let g:fs_spike_names = []
        call ConvertPathToName(split(globpath(g:fs_build_path, '*'), '\n'), g:fs_config_names)
        call ConvertPathToName(split(globpath(g:fs_module_path, '*'), '\n'), g:fs_module_names)
        call ConvertPathToName(split(globpath(g:fs_spike_path, '*'), '\n'), g:fs_spike_names)
        
        " Convert paths for module specific spikes
        for module_name in g:fs_module_names
            let g:fs_module_spike_names[module_name] = []
            call ConvertPathToName(split(globpath(g:fs_module_path . module_name . '/spikes/', '*'), '\n'), g:fs_module_spike_names[module_name])
        endfor
    endfunction
" }}}

" Mappings {{{
    nmap <Leader>bd :exe BuildAll('Debug')<CR>
    nmap <Leader>br :exe BuildAll('Release')<CR>
    nmap <Leader>bi :exe BuildAll('RelWithDebInfo')<CR>
    nmap <Leader>bo :exe BuildAll('MinSizeRel')<CR>
    nmap <Leader>bl :exe BuildProject(g:fs_last_config, g:fs_last_project)<CR>
    nmap <Leader>dl :exe DebugProject(g:fs_last_config, g:fs_last_project)<CR>
    nmap <Leader>dt :exe ToggleDebugTerminalActive()<CR>

    nmap <Leader>mb :Unite -silent menu:build<CR>
    nmap <Leader>md :Unite -silent menu:debug<CR>
    nmap <Leader>mt :Unite -silent menu:test<CR>
    nmap <Leader>ms :Unite -silent menu:select_project<CR>
    nmap <Leader>mc :Unite -silent menu:select_config<CR>
    nmap <Leader>mi :Unite -silent menu:config_ignore<CR>
" }}}

" Functions {{{ 
    function! BuildAll(config_name)
        call SaveLastConfig(a:config_name)
        call SaveLastProject('all')
        call BuildProject(g:fs_last_config, g:fs_last_project)
    endfunction

    function! BuildAllAndTest(config_name)
        call SaveLastConfig(a:config_name)
        call SaveLastProject('all test')
        call ExecuteMakeTargets(a:config_name, g:fs_last_project)
    endfunction

    function! BuildProject(config_name, project_name)
        call SaveLastConfig(a:config_name)
        call SaveLastProject(a:project_name)
        call ExecuteMakeTargets(a:config_name, a:project_name)
    endfunction

    function! BuildAndTestProject(config_name, project_name)
        let a:make_targets = a:project_name . ' ' . a:project_name . ' test'
        call SaveLastConfig(a:config_name)
        call SaveLastProject(a:make_targets)
        call ExecuteMakeTargets(a:config_name, a:make_targets)
    endfunction

    function! ExecuteMakeTargets(config_name, make_targets)
        exe 'let $CTEST_OUTPUT_ON_FAILURE = 1'
        exe ':make -C "' . g:fs_build_path . a:config_name . '" ' . a:make_targets
        exe ':copen'
    endfunction

    function! DebugProject(config_name, project_name)
        call SaveLastConfig(a:config_name)
        call SaveLastProject(a:project_name)
        call StartPyclewn(g:fs_build_path . a:config_name, a:project_name, g:fs_debug_terminal_active)
    endfunction

    function! SaveLastConfig(config_name)
        let g:fs_last_config = a:config_name
        let i = -1
        for candidate in g:unite_source_menu_menus.build.command_candidates
            let i = i + 1
            if candidate[0] =~ "Build All Last"
                call remove(g:unite_source_menu_menus.build.command_candidates, i)
            endif
        endfor
        call insert(g:unite_source_menu_menus.build.command_candidates,
                    \ ['Build All Last (' . g:fs_last_config . ')',
                    \ 'exe BuildAll("' . g:fs_last_config . '")'],
                    \ 1)
    endfunction

    function! SaveLastProject(project_name)
        let g:fs_last_project = a:project_name
        let i = -1
        for candidate in g:unite_source_menu_menus.build.command_candidates
            let i = i + 1
            if candidate[0] =~ "Build Project Last"
                call remove(g:unite_source_menu_menus.build.command_candidates, i)
            endif
        endfor
        call insert(g:unite_source_menu_menus.build.command_candidates,
                    \ ['Build Project Last (' . g:fs_last_project . ' - ' . g:fs_last_config . ')',
                    \ 'exe BuildProject("' . g:fs_last_config . '", "' . g:fs_last_project . '")'],
                    \ 0)
    endfunction

    function! IgnoreConfig(config_name)
        if index(g:FS_IGNORED_CONFIG_NAMES, a:config_name) == -1
            call add(g:FS_IGNORED_CONFIG_NAMES, a:config_name)
            call GenerateMenus()
        endif
    endfunction

    function! UnignoreConfig(config_name)
        let i = index(g:FS_IGNORED_CONFIG_NAMES, a:config_name)
        if i != -1
            call remove(g:FS_IGNORED_CONFIG_NAMES, i)
            call GenerateMenus()
        endif
    endfunction

    function! ToggleDebugTerminalActive()
        let g:fs_debug_terminal_active = !g:fs_debug_terminal_active
        echo 'Debug Terminal Active = ' . g:fs_debug_terminal_active
    endfunction
" }}}

" Menus {{{
    function! GenerateMenus()
        call InitNameLists()
        let g:unite_source_menu_menus = {}

        " Remove ignored configs from menus.
        let a:config_names = []
        for config_name in g:fs_config_names
            if index(g:FS_IGNORED_CONFIG_NAMES, config_name) == -1
                call add(a:config_names, config_name)
            endif
        endfor

    " Build Menu {{{
        let g:unite_source_menu_menus.build = { 'description' : 'Build Menu' }
        let g:unite_source_menu_menus.build.command_candidates = []
        call SaveLastProject(g:fs_last_project)
        call SaveLastConfig(g:fs_last_config)

        for config_name in a:config_names
            call add(g:unite_source_menu_menus.build.command_candidates,
                        \ ['Build All - ' . config_name,
                        \ 'exe BuildAll("' . config_name . '")'])
        endfor

        for config_name in a:config_names
            call add(g:unite_source_menu_menus.build.command_candidates,
                        \ ['Build All And Test - ' . config_name,
                        \ 'exe BuildAllAndTest("' . config_name . '")'])
        endfor

        for module_name in g:fs_module_names
            for config_name in a:config_names
                call add(g:unite_source_menu_menus.build.command_candidates,
                            \ ['Build ' . module_name .' - '. config_name,
                            \ 'exe BuildProject("' . config_name . '", "' . module_name . '")'])
            endfor
        endfor

        for module_name in g:fs_module_names
            for config_name in a:config_names
                if isdirectory(expand(g:fs_module_path . module_name . '/tests'))
                    call add(g:unite_source_menu_menus.build.command_candidates,
                                \ ['Build ' . module_name .'/test - '. config_name,
                                \ 'exe BuildProject("' . config_name . '", "' . module_name . '-test")'])
                endif
            endfor
        endfor

        for module_name in g:fs_module_names
            for spike_name in g:fs_module_spike_names[module_name]
                for config_name in a:config_names
                    call add(g:unite_source_menu_menus.build.command_candidates,
                                \ ['Build ' . module_name . '/spike/' . spike_name .' - '. config_name,
                                \ 'exe BuildProject("' . config_name . '", "' . module_name . '-' . spike_name . '")'])
                endfor
            endfor
        endfor

        for spike_name in g:fs_spike_names
            for config_name in a:config_names
                call add(g:unite_source_menu_menus.build.command_candidates,
                            \ ['Build ' . 'spike/' . spike_name .' - '. config_name,
                            \ 'exe BuildProject("' . config_name . '", "spike-' . spike_name . '")'])
            endfor
        endfor

        call add(g:unite_source_menu_menus.build.command_candidates,
                    \ ['Setup Build Directory ',
                    \ 'exe "!' . g:fs_scripts_path . 'setup_build.sh" | exe GenerateMenus()'])

        call add(g:unite_source_menu_menus.build.command_candidates,
                    \ ['Update Build Directory ',
                    \ 'exe "!' . g:fs_scripts_path . 'update_build.sh" | exe GenerateMenus()'])
    " }}}

    " Debug Menu {{{
        let g:unite_source_menu_menus.debug = { 'description' : 'Select Project Menu' }
        let g:unite_source_menu_menus.debug.command_candidates = []

        for module_name in g:fs_module_names
            for config_name in a:config_names
                if isdirectory(expand(g:fs_module_path . module_name . '/tests'))
                    call add(g:unite_source_menu_menus.debug.command_candidates,
                                \ ['Debug ' . module_name .'/test - '. config_name,
                                \ 'exe DebugProject("' . config_name . '", "' . module_name . '-test")'])
                endif
            endfor
        endfor

        for module_name in g:fs_module_names
            for spike_name in g:fs_module_spike_names[module_name]
                for config_name in a:config_names
                    call add(g:unite_source_menu_menus.debug.command_candidates,
                                \ ['Debug ' . module_name . '/spike/' . spike_name .' - '. config_name,
                                \ 'exe DebugProject("' . config_name . '", "' . module_name . '-' . spike_name . '")'])
                endfor
            endfor
        endfor

        for spike_name in g:fs_spike_names
            for config_name in a:config_names
                call add(g:unite_source_menu_menus.debug.command_candidates,
                            \ ['Debug ' . 'spike/' . spike_name .' - '. config_name,
                            \ 'exe DebugProject("' . config_name . '", "spike-' . spike_name . '")'])
            endfor
        endfor

        call add(g:unite_source_menu_menus.debug.command_candidates,
                    \ ['Toggle Debug Terminal Active', 'exe ToggleDebugTerminalActive()'])
    " }}}
    
    " Test Menu {{{
        let g:unite_source_menu_menus.test = { 'description' : 'Test Project Menu' }
        let g:unite_source_menu_menus.test.command_candidates = []

        for module_name in g:fs_module_names
            for config_name in a:config_names
                call add(g:unite_source_menu_menus.test.command_candidates,
                            \ ['Test ' . module_name . ' - '. config_name,
                            \ 'exe BuildAndTestProject("' . config_name . '", "' . module_name . '-test")'])
            endfor
        endfor
    " }}}

    " Select Project Menu {{{
        let g:unite_source_menu_menus.select_project = { 'description' : 'Select Project Menu' }
        let g:unite_source_menu_menus.select_project.command_candidates = []
        
        call add(g:unite_source_menu_menus.select_project.command_candidates,
                    \ ['Select all', 'exe SaveLastProject("all")'])
        call add(g:unite_source_menu_menus.select_project.command_candidates,
                    \ ['Select test', 'exe SaveLastProject("test")'])
        call add(g:unite_source_menu_menus.select_project.command_candidates,
                    \ ['Select all test', 'exe SaveLastProject("all test")'])

        for module_name in g:fs_module_names
            call add(g:unite_source_menu_menus.select_project.command_candidates,
                        \ ['Select ' . module_name, 'exe SaveLastProject("' . module_name . '")'])
            
            " tests are optional. Only add test target if it exists.
            if isdirectory(expand(g:fs_module_path . module_name . '/tests'))
                call add(g:unite_source_menu_menus.select_project.command_candidates,
                            \ ['Select ' . module_name . '/test', 'exe SaveLastProject("' . module_name . '-test")'])
            endif

            for spike_name in g:fs_module_spike_names[module_name]
                call add(g:unite_source_menu_menus.select_project.command_candidates,
                            \ ['Select ' . module_name . '/spike/' . spike_name,
                            \ 'exe SaveLastProject("' . module_name . '-' . spike_name . '")'])
            endfor
        endfor

        for spike_name in g:fs_spike_names
            call add(g:unite_source_menu_menus.select_project.command_candidates,
                        \ ['Select spike/' . spike_name,
                        \ 'exe SaveLastProject("spike-' . spike_name . '")'])
        endfor
    " }}}
    
    " Select Config Menu {{{
        let g:unite_source_menu_menus.select_config = { 'description' : 'Select Config Menu' }
        let g:unite_source_menu_menus.select_config.command_candidates = []
        
        for config_name in g:fs_config_names
            call add(g:unite_source_menu_menus.select_config.command_candidates,
                        \ ['Select ' . config_name,
                        \ 'exe SaveLastConfig("' . config_name . '") | exe SaveLastProject("' . g:fs_last_project . '")'])
        endfor
    " }}}

    " Config Ignore Menu {{{
        let g:unite_source_menu_menus.config_ignore = { 'description' : 'Config Ingore Menu' }
        let g:unite_source_menu_menus.config_ignore.command_candidates = []

        for config_name in a:config_names
            call add(g:unite_source_menu_menus.config_ignore.command_candidates,
                        \ ['Ignore ' . config_name,
                        \ 'exe IgnoreConfig("' . config_name . '")'])
        endfor

        for config_name in g:FS_IGNORED_CONFIG_NAMES
            call add(g:unite_source_menu_menus.config_ignore.command_candidates,
                        \ ['Un-Ignore ' . config_name,
                        \ 'exe UnignoreConfig("' . config_name . '")'])
        endfor
    " }}}
    endfunction
" }}}

call InitFSBuildMenu()
