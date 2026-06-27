# Daytime

A system that determines the time of day.

## Global architecture

This is a tick-based system. Each frame, 1 tick is added up to 24,000.

When it exceeds 24,000, 24,000 is subtracted.

## Day / night

Night occurs between 18,000 and 6,000 ticks.

A new save starts at 6,000 ticks, at the beginning of the day.

The in-game bed allows the player to reset the time back to 6,000 ticks.
