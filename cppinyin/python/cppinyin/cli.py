import logging
import tempfile

try:
    import click
except ImportError:
    print("Please run")
    print("  pip install click")
    print("before you continue")
    raise

import importlib_resources
from pathlib import Path
from cppinyin import Encoder


def load_dict(user_dict: Path):
    assert user_dict.is_file(), f"File not exists, filename : {user_dict}"
    lines = []
    user_phrases = set()
    with open(user_dict, "r") as fi:
        for line in fi:
            toks = line.strip().split()
            if len(toks) < 3:
                logging.error(
                    "Each line in vocab should contain at lease three items "
                    "(seperate by space), "
                    "the first one is Chinese word/character, the second one is "
                    f"score, the others are the corresponding pinyins given : {line}"
                )
                raise
            try:
                score = float(toks[1])
            except ValueError:
                logging.error(
                    "Each line in vocab should contain at lease three items "
                    "(seperate by space), "
                    "the first one is Chinese word/character, the second one is "
                    f"score, the others are the corresponding pinyins given : {line}"
                )
                raise
            user_phrases.add(toks[0].strip())
            lines.append(line)
    return lines, user_phrases


def get_dict_path(dict_path: Path, user_dict_path: Path):
    if user_dict_path is not None or dict_path is not None:
        ref = importlib_resources.files("cppinyin") / "resources/pinyin.raw"
        with importlib_resources.as_file(ref) as path:
            default_vacab = path

        dict_lines = []
        user_dict_lines = []
        user_phrases = set()
        if dict_path is not None:
            dict_lines, _ = load_dict(dict_path)
        else:
            dict_lines, _ = load_dict(default_vacab)

        if user_dict_path is not None:
            user_dict_lines, user_phrases = load_dict(user_dict_path)

        temp_dict_path = ".cppinyin.tmp"

        with open(temp_dict_path, "w") as fo:
            for line in user_dict_lines:
                fo.write(line)
            for line in dict_lines:
                toks = line.strip().split()
                if toks[0].strip() in user_phrases:
                    continue
                else:
                    fo.write(line)
        return temp_dict_path
    else:
        ref = importlib_resources.files("cppinyin") / "resources/pinyin.dict"
        with importlib_resources.as_file(ref) as path:
            default_vacab = path
        return str(default_vacab)


@click.group()
def cli():
    """
    The shell entry point to cppinyin.
    """
    logging.basicConfig(
        format="%(asctime)s %(levelname)s [%(filename)s:%(lineno)d] %(message)s",
        level=logging.INFO,
    )


@cli.command(name="build")
@click.argument("output", type=click.Path())
@click.option(
    "--dict-path",
    type=Path,
    help="The path to the dictionary, if not provided using default.",
)
@click.option(
    "--user-dict-path", type=Path, help="The path to user customized dict."
)
def build(output: Path, dict_path: Path, user_dict_path: Path):
    """
    Build raw dictionary (in text format) to binary format.

    Note:
      The user_dict has a higher priority than dict (also default dict).
    """
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    encoder = Encoder(get_dict_path(dict_path, user_dict_path))
    encoder.save(str(output))


@cli.command(name="encode")
@click.argument("input", type=str)
@click.option(
    "--dict-path",
    type=Path,
    help="The path to the dictionary, if not provided using default.",
)
@click.option(
    "--user-dict-path", type=Path, help="The path to user customized dict."
)
@click.option(
    "--tone",
    type=click.Choice(["normal", "number", "none"]),
    default="number",
    help="Choose the tone style of pinyin",
)
@click.option(
    "--partial",
    is_flag=True,
    show_default=True,
    default=False,
    help="Whether to split pinyins into initials and finals or not.",
)
def encode(
    input: str,
    dict_path: Path,
    user_dict_path: Path,
    tone: str,
    partial: bool,
):
    """
    Encode a input Chinese sentence into pinyin sequences given dictionary
    (if not provided will use the default one) and user defined dictionary.

    The input could be a string or a file path.

    Note:
      The user_dict has a higher priority than dict (also default dict).
    """
    encoder = Encoder(get_dict_path(dict_path, user_dict_path))
    if Path(input).is_file():
        with open(input, "r") as fi:
            for line in fi:
                pinyin = " ".join(
                    encoder.encode(line.strip(), tone=tone, partial=partial)
                )
                click.echo(f"{line.strip()}\t{pinyin}")
    else:
        click.echo(
            " ".join(encoder.encode(input.strip(), tone=tone, partial=partial))
        )
