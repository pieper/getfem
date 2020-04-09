# -*- coding: utf-8 -*-
#
# GetFEM++ documentation build configuration file.
#
# This file is execfile()d with the current directory set to its containing
# dir.
#
# The contents of this file are pickled, so don't put values in the namespace
# that aren't pickleable (module imports are okay, they're removed
# automatically).
#
# All configuration values have a default; values that are commented out
# serve to show the default.

import os
import sys
import time

###########################################################################
from getfem import getfem_env

# If your extensions are in another directory, add it here. If the directory
# is relative to the documentation root, use os.path.abspath to make it
# absolute, like shown here.
sys.path.append(os.path.abspath('../tools/sphinxext')) # sphinx
sys.path.append('../../../interface/src/python/')      # getfem

user_preamble = '''
\n% begin user_preamble:
\\usepackage{mathrsfs}
\\usepackage{amsmath}
\\usepackage{amssymb}
% end user_preamble
'''

autoclass_content = "both"

_stdauthor = getfem_env('authors')
###########################################################################

# General configuration
# ---------------------

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = ['sphinx.ext.mathjax','sphinx.ext.autodoc',
              'sphinx.ext.coverage',
              'sphinx.ext.doctest']

# The suffix of source filenames.
#source_suffix = '.rst'

# The encoding of source files.
source_encoding = 'utf-8'

# The master toctree document.
master_doc = 'contents'

# List of documents that shouldn't be included in the build.
#unused_docs = []

# List of directories, relative to source directory, that shouldn't be
# searched for source files.
#exclude_trees = []

# A list of directory names that are to be excluded from any recursive
# operation Sphinx performs.
#exclude_dirnames = []

# Directories in which to search for additional Sphinx message catalogs
# relative to the source directory
#locale_dirs = []

gettext_compact = False
locale_dirs = ['locale/']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['.templates']

# A string with the fully-qualified name of a callable (or simply a class)
# that returns an instance of TemplateBridge.
#template_bridge = ''

# A string of reStructuredText that will be included at the end of every
# source file that is read.
#rst_epilog = ''

# The reST default role (used for this markup: `text`) to use for all documents.
#default_role = None

# If true, keep warnings as “system message” paragraphs in the built
# documents.
keep_warnings = True

# A list of prefixes that are ignored for sorting the module index.
modindex_common_prefix = ['getfem']

# Project information
# -------------------

# The documented project’s name.
project = getfem_env('project')

# A copyright statement in the style '2008, Author Name'.
copyright = getfem_env('copyright')

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = getfem_env('version')
# The full version, including alpha/beta/rc tags.
release = getfem_env('release')

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#language = 'en'

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''

# Else, today_fmt is used as the format for a strftime call.
#today_fmt = '%B %d, %Y'

# The default language to highlight source code in.
#highlight_language = 'python'

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# If true, '()' will be appended to :func: etc. cross-reference text.
add_function_parentheses = False

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
add_module_names = False

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
#show_authors = False


# Options for HTML output
# -----------------------

# The "theme" that the HTML output should use.
#html_theme = 'default'

# A dictionary of options that influence the look and feel of the selected
# theme. These are theme-specific.
#html_theme_options = {}

# A list of paths that contain custom themes, either as subdirectories or
# as zip files, relative to this directory.
#html_theme_path = []

# The style sheet to use for HTML and HTML Help pages. A file of that name
# must exist either in Sphinx' static/ path, or in one of the custom paths
# given in html_static_path.
html_style = 'getfem.css'

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
html_title = getfem_env('project')

# A shorter title for the navigation bar.  Default is the same as html_title.
#html_short_title = html_title

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
html_logo = '.static/logo_getfem_small.png'

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
html_favicon = '.static/favicon.ico'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['.static']

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
#html_last_updated_fmt = None
#html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
#html_sidebars = {
#    'index': 'indexsidebar.html',
#}

# Additional templates that should be rendered to pages, maps page names to
# template names.
html_additional_pages = {
    'download': 'download.html',
    'gmm': 'gmm.html',
    'index': 'indexcontent.html',
}

# If false, no module index is generated.
html_use_modindex = False

# If false, no index is generated.
html_use_index = True

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, the reST sources are included in the HTML build as _sources/<name>.
html_copy_source = False

# If true (and html_copy_source is true as well), links to the reST sources
# will be added to the sidebar.
html_show_sourcelink = False

# If nonempty, an OpenSearch description file will be output, and all pages
# will contain a <link> tag referring to it.  The value of this option must
# be the base URL from which the finished HTML is served.
#html_use_opensearch = ''

# If nonempty, this is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = '.html'

# Suffix for generated links to HTML files.
#html_link_suffix = html_file_suffix

# A string with the fully-qualified name of a HTML Translator class, that is,
# a subclass of Sphinx.HTMLTranslator, that is used to translate document
# trees to HTML.
#html_translator_class = None

#If true, “Created using Sphinx” is shown in the HTML footer.
html_show_sphinx = False

# Output file base name for HTML help builder.
htmlhelp_basename = 'getfem' + release.replace('.', '')

# Options for LaTeX output
# ------------------------

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, document class [howto/manual]).
latex_documents = [
    ('tutorial/index', 'getfem_tutorial.tex',
     'Tutorial', _stdauthor, 'manual', False),
    ('python/index', 'python_interface.tex',
     'Python Interface', 'Luis Saavedra', 'manual', False),
    ('matlab/index', 'matlab_interface.tex',
     'Matlab Interface', _stdauthor, 'manual', False),
    ('scilab/index', 'scilab_interface.tex',
     'Scilab Interface', 'Yann Colette, ' + _stdauthor, 'manual', False),
    ('userdoc/index', 'getfem_userdoc.tex',
     'User Documentation', _stdauthor, 'manual', False),
    ('project/index', 'getfem_project.tex',
     'Description of the Project', _stdauthor, 'manual', False),
    ('gmm/index', 'gmm_userdoc.tex',
     'Gmm++ user documentation', 'Yves Renard', 'manual', False),
]

# The name of an image file (relative to this directory) to place at the top of
# the title page.
latex_logo = '.static/logogetfem.png'

# For "manual" documents, if this is true, then toplevel headings are parts,
# not chapters.
#latex_use_parts = False

# Documents to append as an appendix to all manuals.
#latex_appendices = []

# If false, no module index is generated.
#latex_use_modindex = True

# A dictionary that contains LaTeX snippets that override those Sphinx usually
# puts into the generated .tex files.
# Keep in mind that backslashes must be doubled in Python string literals to
# avoid interpretation as escape sequences.
#
#  'papersize' : 'a4paper' or 'letterpaper', default: 'letterpaper'
#  'pointsize' : '10pt', '11pt' or '12pt', default: '10pt'
#  'babel'     : "babel" package inclusion, default: '\\usepackage{babel}'
#  'fontpkg'   : font package inclusion, default '\\usepackage{times}'
#  'fncychap'  : Inclusion of the "fncychap" package, default '\\usepackage[Bjarne]{fncychap}' 
#  'preamble'  : Additional preamble content, default empty.
#  'footer'    : Additional footer content (before the indices), default empty.
latex_elements = {
# The paper size ('letterpaper' or 'a4paper').
'papersize': 'a4paper',

# The font size ('10pt', '11pt' or '12pt').
'pointsize': '11pt',

# Additional stuff for the LaTeX preamble.
    'preamble': user_preamble
}

# A list of file names, relative to the configuration directory, to copy to
# the build directory when building LaTeX output.
#latex_additional_files = []
