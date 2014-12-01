{
    'variables': {
        'CassowaryPlugin_include_dir': '.',
        'CassowaryPlugin_files': [
            'ableton/cassowary/CassowaryPlugin.cpp',
            'ableton/cassowary/CassowaryPlugin.hpp',
            'ableton/cassowary/Solver.cpp',
            'ableton/cassowary/Solver.hpp',
            'ableton/cassowary/Variable.cpp',
            'ableton/cassowary/Variable.hpp',
            '../third-party/rhea/rhea/abstract_variable.cpp',
            '../third-party/rhea/rhea/symbolic_weight.cpp',
            '../third-party/rhea/rhea/linear_expression.cpp',
            '../third-party/rhea/rhea/tableau.cpp',
            '../third-party/rhea/rhea/simplex_solver.cpp'
        ],
    },

    'targets': [
        {
            'target_name': 'CassowaryPlugin',
            'type': '<(LibraryType)',

            'variables': {
                'QtPluginUri': 'Ableton.Cassowary',
            },

            'dependencies': [
                'QtPluginSettings',
                'XQtQml',
                'XQtQuick',
            ],

            'include_dirs': [
                '<(CassowaryPlugin_include_dir)',
                '../third-party/rhea'
            ],

            'sources': [
                '<@(CassowaryPlugin_files)',
            ],
        },
    ],
}
