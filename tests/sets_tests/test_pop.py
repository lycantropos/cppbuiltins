import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         equivalence)
from . import strategies


@given(strategies.non_empty_sets_pairs)
def test_defaults(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    alternative_result, native_result = alternative.pop(), native.pop()

    assert equivalence(alternative_result not in alternative,
                       native_result not in native)


@given(strategies.empty_sets_pairs)
def test_empty(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    with pytest.raises(KeyError):
        alternative.pop()
    with pytest.raises(KeyError):
        native.pop()
