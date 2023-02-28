# Building the MoveTK documentation
MoveTK provides its documentation by employing [Doxygen](https://www.doxygen.nl/). To be able to support concepts, we require a fairly new version of Doxygen, namely version 1.9.6.

To have more tight control over the documenation, we have a separate script to build the documentation, ``build-documentation.py`` (requires Python 3). This script expects the version of the built MoveTK to be provided via the command line and optionally accepts an ``--output-folder`` option with which you can specify an output folder. Otherwise, the default output folder is ``<movetk_root>/docs`` with ``<movetk_root>`` the root of the MoveTK repository.

## Doc build process
The build script fills in the templates required by Doxygen (``templates/*.html,templates/layout.xml, movetk-doxy-config.cfg.in``). All required files and resources are then written to an intermediate folder ``<movetk_root>/build-docs``, which are then used to generate the documentation.

## Custom citation script
To avoid having to install LaTeX/BibTex, MoveTK uses a citation system that employs [Citation.js](https://citation.js.org/). To cite an element, add the bibliography BibTex item in the ``movetk.bib`` file in this folder. Then, cite the element using the custom ``\mtkCite{<cite_key>}`` command. When such a command is present on the page, a list of the bibliography items used will be added to the end of the page.

## Tutorials
The tutorials in ``<movetk_root>/tutorials/cpp`` are also included as subpages in the Doxygen-generated documentation. Some care must be taken to keep them both usable on GitHub and in Doxygen, due to differences in the Markdown syntax. ``build-documentation.py`` takes care of this, converting the GH version to the Doxygen version.