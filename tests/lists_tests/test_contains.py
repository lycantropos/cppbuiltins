from typing import Any

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         equivalence)
from . import strategies


@given(strategies.lists_pairs, strategies.objects)
def test_basic(pair: AlternativeNativeListsPair, value: Any) -> None:
    alternative, native = pair

    assert equivalence(value in alternative, value in native)
