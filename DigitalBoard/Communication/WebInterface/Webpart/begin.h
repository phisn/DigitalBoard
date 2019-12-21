#pragma once

#define COMMUNICATION_WEBPART_BEGIN R"__(
<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="content-type" content="text/html; charset=UTF-8">
		<title></title>
	</head>)__"

namespace Communication
{
	namespace Webpart
	{
		const char* const begin = PROGMEM COMMUNICATION_WEBPART_BEGIN;
		const size_t begin_size = sizeof(COMMUNICATION_WEBPART_BEGIN);
	}
}
