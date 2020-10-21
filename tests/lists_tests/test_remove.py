from typing import (Any,
                    Tuple)

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeListsPair,
                         are_alternative_native_lists_equal)
from . import strategies


@given(strategies.non_empty_lists_pairs_with_their_elements)
def test_basic(pair_with_value: Tuple[AlternativeNativeListsPair, Any]
               ) -> None:
    (alternative, native), value = pair_with_value

    alternative_result, native_result = (alternative.remove(value),
                                         native.remove(value))

    assert alternative_result is native_result is None
    assert are_alternative_native_lists_equal(alternative, native)


@given(strategies.lists_pairs_with_non_their_elements)
def test_missing(pair_with_value: Tuple[AlternativeNativeListsPair, Any]
                 ) -> None:
    (alternative, native), value = pair_with_value

    with pytest.raises(ValueError):
        alternative.remove(value)
    with pytest.raises(ValueError):
        native.remove(value)
