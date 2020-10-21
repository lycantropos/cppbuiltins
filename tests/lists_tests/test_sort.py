from typing import (Callable,
                    Optional)

from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         Domain,
                         Range,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.lists_pairs, strategies.keys, strategies.booleans)
def test_basic(pair: AlternativeNativeListsPair,
               key: Optional[Callable[[Domain], Range]],
               reverse: bool) -> None:
    alternative, native = pair

    alternative_result, native_result = (alternative.sort(key=key,
                                                          reverse=reverse),
                                         native.sort(key=key,
                                                     reverse=reverse))

    assert alternative_result is native_result is None
    assert are_alternative_native_lists_equal(alternative, native)
