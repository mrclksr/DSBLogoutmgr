/*-
 * Copyright (c) 2017 Marcel Kaiser. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

dsbcfg_vardef_t vardefs[] = {
	{
		"lock", DSBCFG_VAR_STRING, CFG_LOCK,
		DSBCFG_VAL(CFG_LOCK_CMD)
	},
	{
		"logout", DSBCFG_VAR_STRING, CFG_LOGOUT,
		DSBCFG_VAL(CFG_LOGOUT_CMD)
	},
	{
		"shutdown", DSBCFG_VAR_STRING, CFG_SHUTDOWN,
		DSBCFG_VAL(CFG_SHUTDOWN_CMD)
	},
	{
		"reboot", DSBCFG_VAR_STRING, CFG_REBOOT,
		DSBCFG_VAL(CFG_REBOOT_CMD)
	},
	{
		"suspend",  DSBCFG_VAR_STRING, CFG_SUSPEND,
		DSBCFG_VAL(CFG_SUSPEND_CMD)
	}
};

