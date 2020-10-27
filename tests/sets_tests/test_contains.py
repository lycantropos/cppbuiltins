from typing import Any

from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         equivalence)
from . import strategies


@given(strategies.sets_pairs, strategies.objects)
def test_basic(pair: AlternativeNativeSetsPair, value: Any) -> None:
    alternative, native = pair

    assert equivalence(value in alternative, value in native)
