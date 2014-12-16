{
    'variables': {
        'CassowaryPlugin_include_dir': '.',
        'CassowaryPlugin_files': [
            '../third-party/rhea/rhea/abstract_variable.cpp',
            '../third-party/rhea/rhea/linear_expression.cpp',
            '../third-party/rhea/rhea/simplex_solver.cpp',
            '../third-party/rhea/rhea/symbolic_weight.cpp',
            '../third-party/rhea/rhea/tableau.cpp',
            'ableton/cassowary/CassowaryPlugin.cpp',
            'ableton/cassowary/CassowaryPlugin.hpp',
            'ableton/cassowary/Constraint.cpp',
            'ableton/cassowary/Constraint.hpp',
            'ableton/cassowary/ConstraintItem.cpp',
            'ableton/cassowary/ConstraintItem.hpp',
            'ableton/cassowary/Context.cpp',
            'ableton/cassowary/Context.hpp',
            'ableton/cassowary/Contextual.cpp',
            'ableton/cassowary/Contextual.hpp',
            'ableton/cassowary/Edit.cpp',
            'ableton/cassowary/Edit.hpp',
            'ableton/cassowary/Solver.cpp',
            'ableton/cassowary/Solver.hpp',
            'ableton/cassowary/SolverItem.cpp',
            'ableton/cassowary/SolverItem.hpp',
            'ableton/cassowary/SolverProxy.cpp',
            'ableton/cassowary/SolverProxy.hpp',
            'ableton/cassowary/Stay.cpp',
            'ableton/cassowary/Stay.hpp',
            'ableton/cassowary/Strength.cpp',
            'ableton/cassowary/Strength.hpp',
            'ableton/cassowary/Variable.cpp',
            'ableton/cassowary/Variable.hpp',
            'ableton/estd/functional.hpp',
        ],
    },

    'targets': [
        {
            'target_name': 'CassowaryPlugin',
            'type': '<(LibraryType)',

            'variables': {
                'QtPluginUri': 'Ableton.Cassowary',
                'ExtraMocArgs': [
                    '-I', '<(CassowaryPlugin_include_dir)',
                ],
            },

            'dependencies': [
                'QtPluginSettings',
                'XQtQml',
                'XQtQuick',
                'XBoostLibCommons'
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
