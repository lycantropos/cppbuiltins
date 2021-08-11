from hypothesis import (HealthCheck,
                        Verbosity,
                        settings)

settings.register_profile('default',
                          deadline=None,
                          suppress_health_check=[HealthCheck.filter_too_much,
                                                 HealthCheck.too_slow],
                          verbosity=Verbosity.verbose)
