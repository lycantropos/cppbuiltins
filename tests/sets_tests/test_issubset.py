from typing import (Any,
                    List)

from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         equivalence)
from . import strategies


@given(strategies.sets_pairs, strategies.objects_lists)
def test_basic(pair: AlternativeNativeSetsPair, other: List[Any]) -> None:
    alternative, native = pair

    assert equivalence(alternative.issubset(other), native.issubset(other))
