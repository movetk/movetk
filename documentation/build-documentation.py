import typing
from pathlib import Path
from argparse import ArgumentParser
import subprocess
import re
import xml.etree.ElementTree as ET
import os
import shutil


class Version:
    def __init__(self, major: int, minor: int, patch: int = -1) -> None:
        self.major = major
        self.minor = minor
        self.patch = patch if patch >= 0 else 0
        self.patch_was_specified = patch >= 0

    def less(self, other_version: "Version"):
        if self.major < other_version.major:
            return True
        if self.minor < other_version.minor:
            return True
        if self.patch < other_version.patch:
            return True
        return False

    def equals(self, other_version):
        return not self.less(other_version) and not other_version.less(self)

    def __repr__(self) -> str:
        if self.patch_was_specified:
            return "{}.{}.{}".format(self.major, self.minor, self.patch)
        return "{}.{}".format(self.major, self.minor)

    """Construct a version from a version string.
    :param version_string: The version string. Should be in format
    MAJOR.MINOR or MAJOR.MINOR.PATCH
    """
    @staticmethod
    def from_string(version_string: str) -> "Version":
        match = re.match('([0-9]+)\\.([0-9]+)(\\.[0-9]+)?', version_string)
        if match is None:
            raise Exception(
                'Invalid version string "{}"'.format(version_string))
        patch = -1
        (major, minor, patch_str) = match.groups()
        major = int(major)
        minor = int(minor)
        patch = patch if patch_str is None else int(patch_str[1:])
        return Version(major, minor, patch)


# Global variables
ROOT_PATH = Path(__file__).parent.resolve()  # Folder of the current script
BUILD_PATH = ROOT_PATH / '..' / 'build-docs'


def setup_build_directory():
    """Setup the folder structure in the build directory
    """
    dirs = [
        BUILD_PATH,
        BUILD_PATH/'pages',
        BUILD_PATH/'templates',
        BUILD_PATH/'images',
        BUILD_PATH/'extra_pages',
        BUILD_PATH/'resources'
    ]
    for directory in dirs:
        directory.mkdir(exist_ok=True)


setup_build_directory()

def copy_if_newer(src:Path,dest:Path,verbose=True):
    if not dest.exists() or os.stat(src).st_mtime - os.stat(dest).st_mtime > 0:
        if verbose:
            print('[MoveTK docs] Syncing {} -> {}'.format(src,dest))
        shutil.copy2 (src, dest)

def sync_to_build_folder(directory_name: str,verbose=True):
    if verbose:
        print('[MoveTK docs] Syncing folder {}'.format(directory_name))
    elements = list(os.listdir(ROOT_PATH / directory_name))
    for elem in filter(lambda x: not x.endswith('.in'), elements):
        src = ROOT_PATH / directory_name/elem
        dest = BUILD_PATH / directory_name / elem
        copy_if_newer(src,dest,verbose=verbose)


MOVETK_DOXYGEN_CONFIG = ROOT_PATH / 'movetk-doxy-config.cfg.in'
MOVETK_DOXYGEN_CONFIG_OUT = BUILD_PATH / 'movetk-doxy-config.cfg'
MOVETK_DOXYGEN_LAYOUT_IN = ROOT_PATH / 'templates' / 'layout.xml.in'
MOVETK_DOXYGEN_LAYOUT_OUT = BUILD_PATH / 'templates' / 'layout.xml'
DEFAULT_OUTPUT_FOLDER = ROOT_PATH / '..' / 'docs'
BIBLIOGRAPHY_FILE = ROOT_PATH / 'movetk.bib'
BIBLIOGRAPHY_GEN_SOURCE = ROOT_PATH / 'resources' / 'bibliography.js.in'
BIBLIOGRAPHY_GEN_TARGET = BUILD_PATH / 'resources' / 'bibliography.js'

REQUIRED_DOXYGEN_VERSION = Version(1, 9, 6)


def check_doxygen():
    """Check if doxygen is present and that it has the right version
    """
    result = subprocess.run(['doxygen', '--version'],
                            stdout=subprocess.PIPE, universal_newlines=True)
    if result.returncode != 0:
        raise Exception(
            "Got non-zero exit code when running doxygen. Make sure it is installed")
    version_string = str(result.stdout).strip()
    doxy_ver = Version.from_string(version_string)
    if doxy_ver.less(REQUIRED_DOXYGEN_VERSION):
        raise Exception("Need at least doxygen version {}".format(
            REQUIRED_DOXYGEN_VERSION))


class DocumentationParameters:
    def __init__(self, output_folder: str = '', movetk_version: str = '', output_folder_name:str='') -> None:
        self.output_folder = output_folder
        self.movetk_version = movetk_version
        self.html_output = movetk_version if len(output_folder_name) == 0 else output_folder_name

    def get_value(self, key: str):
        if hasattr(self, key):
            return getattr(self, key)
        raise Exception("Unknown documentation parameter '{}'".format(key))


def prepare_config(doc_params: DocumentationParameters):
    # Read the config template file
    with MOVETK_DOXYGEN_CONFIG.open() as config_template_stream:
        config_template = config_template_stream.read()
    # Replace anything between {{}} with the appropriate DocumentationParameters
    # member
    regex = re.compile(r"\{\{\s*([0-9a-zA-Z_]+)\s*\}\}")

    def substitution_func(match_obj):
        key = match_obj.group(1).strip()
        print('[MoveTK docs] Replacing "{}" in config'.format(key))
        return doc_params.get_value(key)
    config_template = re.sub(regex, substitution_func, config_template)
    # Write to output file, to be used with doxygen
    with MOVETK_DOXYGEN_CONFIG_OUT.open('w') as config_out_stream:
        config_out_stream.write(config_template)


def run_doxygen():
    """
    Generate the documentation using doxygen
    """
    subprocess.run([
        'doxygen',
        '-q',
        str(MOVETK_DOXYGEN_CONFIG_OUT)
    ], check=True, cwd=str(BUILD_PATH))


class LayoutUpdater:
    def __init__(self, layout_file: Path):
        self.tree = ET.parse(layout_file)
        self.root = self.tree.getroot()

    def write_updated_layout(self, output_file: Path):
        self.tree.write(str(output_file), xml_declaration=False)

    class Tutorial:
        def __init__(self,ref:str='', title:str='') -> None:
            # Ref to use within Doxygen
            self.ref = ref
            self.title = title
    
    @staticmethod
    def _get_headers(markdown_files:list[Path])->list[str]:
        output:list[str] = []
        for file  in markdown_files:
            header_line = ''
            with open(file) as f:
                # Read one line
                for line in f:
                    header_line=line
                    break
            # Parse header, assume of an h1 Markdown shape, i.e. '# Title ...'
            match = re.match(r'#\s*([0-9A-Za-z_\-\s]+)\s*([^0-9A-Za-z_\-])?', header_line)
            assert match is not None
            title = match.group(1).strip()
            output.append(title)
        return output
    @staticmethod
    def _get_tutorials(tutorials_folder:Path)->list[Tutorial]:
        output:list[LayoutUpdater.Tutorial] = []
        tutorials = sorted(
            filter(lambda x:x.endswith('.md'),list(os.listdir(tutorials_folder)))
                           )
        for tut in tutorials:
            header_line = ''
            with open(tutorials_folder/tut) as f:
                # Read one line
                for line in f:
                    header_line=line
                    break
            # Parse header, assume of an h1 Markdown shape, i.e. '# Title ...'
            match = re.match(r'#\s*([0-9A-Za-z_\-\s]+)\s*([^0-9A-Za-z_\-])?', header_line)
            assert match is not None
            title = match.group(1).strip()
            # TODO: this ref should be md_<subpath_to_file>_<file_name_no_ext>
            output.append(LayoutUpdater.Tutorial(
                'md_pages_{}'.format(tut.replace('.md','').replace('-','_')),
                title)
                )

        return output
    def add_pages(self, pages_folder: Path):
        namespaces_node = self.root.find("./navindex/tab[@type='mainpage']")
        nav_node = self.root.find('./navindex')
        # Don't include introduction.md, this is the main page
        pages = list(filter(lambda x:x!='introduction.md',os.listdir(pages_folder)))
        # Write tab elements for the tutorials
        prev_node = namespaces_node
        headers = LayoutUpdater._get_headers([pages_folder / p for p in pages])
        for i, page in enumerate(pages):
            print(page)
            new_node = ET.Element('tab', dict(
                type='user',
                title=headers[i],
                # TODO: this ref should be md_<subpath_to_file>_<file_name_no_ext>
                url=str('@ref md_pages_{}'.format(page.replace('-','_').replace('.md','')))
            ))
            LayoutUpdater.insert_after(nav_node, prev_node, new_node)
            prev_node = new_node

    def add_tutorial_pages(self, tutorials_folder: Path):
        namespaces_node = self.root.find("./navindex/tab[@type='namespaces']")
        nav_node = self.root.find('./navindex')
        new_node = ET.Element('tab', {
            'type': 'usergroup',
            'title': 'Tutorials'
        })
        tutorials = LayoutUpdater._get_tutorials(tutorials_folder)
        # Write tab elements for the tutorials
        for i, tut in enumerate(tutorials):
            ET.SubElement(new_node, 'tab', dict(
                type='user',
                title='{} {}'.format(str(i+1),tut.title),
                # TODO: this ref should be md_<subpath_to_file>_<file_name_no_ext>
                url=str('@ref {}'.format(tut.ref))
            ))

        LayoutUpdater.insert_before(nav_node, namespaces_node, new_node)

    @staticmethod
    def get_index(parent, element):
        for idx, child in enumerate(parent):
            if element == child:
                return idx
        raise Exception('Given element not a child of the given parent')

    @staticmethod
    def insert_before(parent, element, element_to_insert):
        LayoutUpdater.insert_relative(parent, element, element_to_insert)

    @staticmethod
    def insert_after(parent, element, element_to_insert):
        LayoutUpdater.insert_relative(parent, element, element_to_insert, 1)

    @staticmethod
    def insert_relative(parent, element, element_to_insert, offset=0):
        idx = LayoutUpdater.get_index(parent, element)
        parent.insert(idx+offset, element_to_insert)

def copy_tutorials(tutorials_path:Path, output_path:Path):
    """Copy tutorial .md files to the output directory.
    Converts Github markdown to Doxygen markdown where necessary

    Args:
        tutorials_path (Path): The path to the tutorials folder
        output_path (Path): The output path for the tutorial files
    """
    for elem in filter(lambda x:x.endswith('.md'),os.listdir(tutorials_path)):
        data = ''
        with open(tutorials_path/elem) as f:
            data = f.read()
        data = data.replace('```c++',r'```{.cpp}')
        with open(output_path/elem,'w') as f:
            f.write(data)

def update_doxygen_layout():
    updater = LayoutUpdater(MOVETK_DOXYGEN_LAYOUT_IN)
    # Add tutorial pages to the layout. They get a separate menu
    updater.add_tutorial_pages(
        ROOT_PATH / '..' / 'tutorials' / 'cpp')
    updater.add_pages(ROOT_PATH / 'pages')
    # Write the resulting layout.
    updater.write_updated_layout(MOVETK_DOXYGEN_LAYOUT_OUT)


def add_bibliography():
    with (BIBLIOGRAPHY_FILE).open() as bib_file:
        bibiliography = bib_file.read()
        # Escape quote characters
        bibiliography = bibiliography.replace('\'', '\\\'')
        bibiliography = bibiliography.replace('\n', '')
        bibiliography = bibiliography.replace('\r', '')
    with BIBLIOGRAPHY_GEN_SOURCE.open() as template:
        template_content = template.read()
        template_content = template_content.replace(
            '$bibliography', bibiliography)
        with BIBLIOGRAPHY_GEN_TARGET.open('w') as template_out:
            template_out.write(template_content)


if __name__ == '__main__':
    arg_parser = ArgumentParser('Generate documentation for MoveTK')
    arg_parser.add_argument('mtk-version', type=str, help='MoveTk version, specified as x.y or x.y.z'
                            'Note that x.y implies x.y.0')
    arg_parser.add_argument('--output-folder', '-o', type=str,
                            help='Define the output folder. Otherwise, ../docs is picked')
    args = arg_parser.parse_args()
    # Check doxygen availability
    check_doxygen()
    # Determine outputfolder and version
    output_folder = Path(
        args.output_folder) if args.output_folder is not None else DEFAULT_OUTPUT_FOLDER.resolve()
    version = Version.from_string(getattr(args, 'mtk-version'))
    # Setup documentation parameters
    doc_params = DocumentationParameters(output_folder=str(
        output_folder), movetk_version=str(version))
    print('[MoveTK docs] Outputting to {}'.format(output_folder))
    # Add bibliography support via javascript
    add_bibliography()

    # Sync resource directories
    sync_to_build_folder('templates')
    sync_to_build_folder('images')
    sync_to_build_folder('extra_pages')
    sync_to_build_folder('pages')
    sync_to_build_folder('resources')

    # Copy tutorial files
    copy_tutorials(ROOT_PATH / '..' / 'tutorials' / 'cpp', BUILD_PATH / 'pages')

    # Update Doxygen layout for custom pages/tutorials
    update_doxygen_layout()

    # Prepare the config file
    prepare_config(doc_params)

    # Run doxygen
    run_doxygen()
