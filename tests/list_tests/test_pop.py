import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs)
def test_basic(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.pop(), native.pop()

    assert alternative_result == native_result
    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.empty_lists_pairs)
def test_empty(pair: AlternativeNativeListsPair) -> None:
    alternative, native = pair

    with pytest.raises(IndexError):
        alternative.pop()
    with pytest.raises(IndexError):
        native.pop()
