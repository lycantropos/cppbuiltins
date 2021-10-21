import pytest
from hypothesis import given

from tests.utils import (AlternativeNativeIntsPair,
                         are_alternative_native_ints_equal)
from . import strategies


@given(strategies.ints_pairs, strategies.single_byte_ints_pairs)
def test_basic(bases_pair: AlternativeNativeIntsPair,
               steps_pair: AlternativeNativeIntsPair) -> None:
    alternative_base, native_base = bases_pair
    alternative_step, native_step = steps_pair

    try:
        alternative_result = alternative_base >> alternative_step
    except ValueError:
        with pytest.raises(ValueError):
            native_base >> native_step
    else:
        native_result = native_base >> native_step

        assert are_alternative_native_ints_equal(alternative_result,
                                                 native_result)
