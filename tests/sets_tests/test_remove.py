from typing import (Any,
                    Tuple)

import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeSetsPair,
                         are_alternative_native_sets_equal)
from . import strategies


@given(strategies.non_empty_sets_pairs_with_their_elements)
def test_basic(pair_with_value: Tuple[AlternativeNativeSetsPair, Any]
               ) -> None:
    (alternative, native), value = pair_with_value

    alternative_result, native_result = (alternative.remove(value),
                                         native.remove(value))

    assert alternative_result is native_result is None
    assert are_alternative_native_sets_equal(alternative, native)


@given(strategies.sets_pairs_with_non_their_elements)
def test_missing(pair_with_value: Tuple[AlternativeNativeSetsPair, Any]
                 ) -> None:
    (alternative, native), value = pair_with_value

    with pytest.raises(KeyError):
        alternative.remove(value)
    with pytest.raises(KeyError):
        native.remove(value)
