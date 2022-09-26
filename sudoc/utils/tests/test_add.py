from load_c import load
import pytest

add_module = load("utils", "add")


def test_load():
    assert add_module is not None


def test_add():
    assert add_module.add(1, 2) == 3
