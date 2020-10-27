from hypothesis import given

from tests.utils import AlternativeNativeSetsPair
from . import strategies


@given(strategies.sets_pairs)
def test_basic(pair: AlternativeNativeSetsPair) -> None:
    alternative, native = pair

    assert bool(alternative) is bool(native)
