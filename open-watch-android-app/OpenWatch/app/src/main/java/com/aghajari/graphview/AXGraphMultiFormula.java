/*
 * Copyright (C) 2021 - Amir Hossein Aghajari
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


package com.aghajari.graphview;

public abstract class AXGraphMultiFormula extends AXGraphFormula{

    protected float[] applyMultiFunction (float x){
        float[] y = multiFunction(transformScaleX * (x + transformX));
        for (int i = 0; i<y.length; i++){
            y[i] = transformScaleY * (y[i]) + transformY;
        }
        return y;
    }

    public abstract float[] multiFunction(float x);

    @Override
    public float function(float x) {
        return Float.POSITIVE_INFINITY; //undefined
    }
}
