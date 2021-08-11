from hypothesis import (HealthCheck,
                        Verbosity,
                        settings)

settings.register_profile('default',
                          deadline=None,
                          suppress_health_check=[HealthCheck.filter_too_much,
                                                 HealthCheck.too_slow],
                          max_examples=10 ** 4,
                          verbosity=Verbosity.verbose)
