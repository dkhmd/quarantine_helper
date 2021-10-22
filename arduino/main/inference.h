#ifndef __inference_h__
#define __inference_h__

#include <Arduino.h>
#include <TensorFlowLite.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/version.h"
#include "model.h"

void inference_setup();
bool inference_exec(float data[], int len);

#endif
