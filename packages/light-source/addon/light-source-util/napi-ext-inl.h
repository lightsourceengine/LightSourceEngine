/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

#pragma once

namespace ls {

template<typename T>
T GetNumberOrDefault(const Napi::Object options, const char* name, T defaultValue) {
    if (!options.Has(name)) {
        return defaultValue;
    }

    auto value{ options.Get(name) };

    if (!value.IsNumber()) {
        return defaultValue;
    }

    return value.As<Napi::Number>();
}

template<typename T>
std::unordered_set<AsyncWork<T>*> AsyncWork<T>::activeWork;

template<typename T>
AsyncWork<T>::AsyncWork(
    Napi::Env env,
    const std::string& resourceName,
    ExecuteFunction execute,
    CompleteFunction complete)
: env(env), execute(execute), complete(complete) {
    napi_value resourceId;
    napi_status status = napi_create_string_latin1(
        env,
        resourceName.c_str(),
        NAPI_AUTO_LENGTH,
        &resourceId);

    if ((status) != napi_ok) {
        throw Napi::Error::New(env);
    }

    status = napi_create_async_work(
        env,
        nullptr,
        resourceId,
        AsyncWork<T>::OnExecute,
        AsyncWork<T>::OnComplete,
        this,
        &this->work);

    if ((status) != napi_ok) {
        throw Napi::Error::New(env);
    }

    AsyncWork<T>::activeWork.insert(this);
    status = napi_queue_async_work(env, this->work);

    if (status != napi_ok) {
        AsyncWork<T>::activeWork.erase(this);
        napi_delete_async_work(env, this->work);
        this->work = nullptr;
    }

    if ((status) != napi_ok) {
        throw Napi::Error::New(env);
    }
}

template<typename T>
AsyncWork<T>::~AsyncWork() {
    AsyncWork<T>::activeWork.erase(this);

    if (this->work) {
        napi_delete_async_work(this->env, this->work);
        this->work = nullptr;
    }
}

template<typename T>
void AsyncWork<T>::OnExecute(napi_env env, void* self) {
    auto asyncWork{ static_cast<AsyncWork<T>*>(self) };

    if (activeWork.find(asyncWork) == activeWork.end()) {
        return;
    }

    try {
        asyncWork->result = asyncWork->execute(asyncWork->env);
        asyncWork->status = napi_ok;
    } catch (std::exception& e) {
        asyncWork->message = e.what();
        asyncWork->status = napi_generic_failure;
    }
}

template<typename T>
void AsyncWork<T>::OnComplete(napi_env env, napi_status status, void* self) {
    auto asyncWork{ static_cast<AsyncWork<T>*>(self) };

    if (activeWork.find(asyncWork) == activeWork.end()) {
        return;
    }

    asyncWork->complete(asyncWork->env, asyncWork->result, asyncWork->status, asyncWork->message);
}

} // namespace ls
