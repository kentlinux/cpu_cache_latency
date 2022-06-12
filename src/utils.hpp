#pragma once

bool CheckHardware();

long ReadMaxCpuFreqInHZ();

double Tick2Nano(unsigned tick, long max_cpu_freq_in_hz);

std::string Tick2String(unsigned tick, long max_cpu_freq_in_hz, int verbose_mode);
