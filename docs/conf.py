# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
import sphinx_rtd_theme

project = 'Niflheim'
copyright = '2025, Ole Holm Nielsen'
author = 'Ole Holm Nielsen'

version = '24.07'
release = '24.07'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

# extensions = []
extensions = [
    'sphinx_rtd_theme',
]
    # 'sphinx.ext.autosectionlabel',

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'en'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# See https://sphinx-rtd-theme.readthedocs.io/en/stable/
html_theme = 'sphinx_rtd_theme'
# html_theme = 'classic'
# html_static_path = ['_static']

html_theme_options = {
        "body_min_width": "0",
        "body_max_width": "none"
}
