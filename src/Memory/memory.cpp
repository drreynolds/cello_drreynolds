// $Id$
// See LICENSE_CELLO file for license and copyright information

/// @file      memory.cpp
/// @author    James Bordner (jobordner@ucsd.edu)
/// @date      Thu Sep  3 16:44:18 PDT 2009
/// @brief     Functions for dynamic memory management

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <new>

#include "cello.h"

#include "error.hpp"
#include "memory.hpp"

Memory Memory::instance_; // (singleton design pattern)

//======================================================================
// FUNCTIONS
//======================================================================
 
void Memory::initialize_() throw ()
{
#ifdef CONFIG_USE_MEMORY
  curr_group_.push(0);
  is_active_ = true;
  group_names_[0] = strdup("");
  group_names_[1] = strdup("memory");

  for (int i=0; i<MEMORY_MAX_NUM_GROUPS + 1; i++) {
    group_names_ [i] = 0;
    available_   [i] = 0;
    bytes_       [i] = 0;
    bytes_high_  [i] = 0;
    new_calls_   [i] = 0;
    delete_calls_[i] = 0;
  }

#endif
}

//----------------------------------------------------------------------

void * Memory::allocate ( size_t bytes ) throw (ExceptionMemoryBadAllocate())
/// @param  bytes   Number of bytes to allocate
/// @return        Pointer to the allocated memory
{
#ifdef CONFIG_USE_MEMORY
  int * buffer = (int *)(malloc(bytes + 2*sizeof(int)));


  if (buffer==0) throw ExceptionMemoryBadAllocate();

  if (is_active_) {
    buffer[0] = bytes;
    buffer[1] = curr_group_.top();

    ++ new_calls_[0] ;
    bytes_[0] += bytes;
    bytes_high_[0] = bytes_[0];

    memory_group_handle current = curr_group_.top();

    if (current != 0) {
      ++ new_calls_[current] ;
      bytes_[current] += bytes;
      bytes_high_[current] = bytes_[current];
    }

  } else {
    buffer[0] = 0;
    buffer[1] = 0;
  }
  return (void *)(buffer + 2);
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

void Memory::deallocate ( void * pointer )
  throw (ExceptionMemoryBadDeallocate())
{
#ifdef CONFIG_USE_MEMORY
  int *buffer = (int *)(pointer) - 2;

  if (is_active_) {
    ++ delete_calls_[0] ;
    bytes_[0] -= buffer[0];

    memory_group_handle current = buffer[1];

    if (current != 0) {
      ++ delete_calls_[current] ;
      bytes_[current] -= buffer[0];
    }
  }

  free(buffer);
#endif
}

//----------------------------------------------------------------------

void Memory::new_group ( memory_group_handle group_id, const char * group_name ) throw ()
/// @param  group_id    non-zero ID of the group
/// @param  group_name  Name of the group
{
#ifdef CONFIG_USE_MEMORY
  if (group_id == 0 || group_id > MEMORY_MAX_NUM_GROUPS) {

    WARNING_MESSAGE("Memory::new_group()","group_id out of range");

  } else {

    group_names_[group_id] = strdup(group_name);

  }
#endif
}

//----------------------------------------------------------------------

void  Memory::begin_group ( memory_group_handle group_id ) throw ()
/// @param  group_id ID of the group
///
/// If number of groups exceeds MEMORY_MAX_NUM_GROUPS_, issue a
/// warning and fall back to "general" group
{
#ifdef CONFIG_USE_MEMORY
  // check for whether we have already called begin_group before

  bool in_range = (group_id <= MEMORY_MAX_NUM_GROUPS);

  if ( in_range ) {

    curr_group_.push(group_id);

  } else { // curr_group_ out of range

    char warning_message [ ERROR_MESSAGE_LENGTH ];

    sprintf (warning_message, "Group %d is out of range [1,%d]\n",
	     group_id, MEMORY_MAX_NUM_GROUPS);

    WARNING_MESSAGE("Memory::begin_group()",warning_message);

  }
#endif
}

//----------------------------------------------------------------------

void Memory::end_group ( memory_group_handle group_id ) throw ()
{
#ifdef CONFIG_USE_MEMORY

  char warning_message [ ERROR_MESSAGE_LENGTH ];

  bool in_range = (group_id <= MEMORY_MAX_NUM_GROUPS);

  if ( in_range ) {

    if (curr_group_.size() > 0) {

      if (curr_group_.top() != group_id) {
	sprintf (warning_message, 
		 "Mismatch between end_group(%d) and group stack top %d\n",
		 group_id,curr_group_.top());

	WARNING_MESSAGE("Memory::end_group",warning_message);
      }

      curr_group_.pop();

    } else {

      sprintf (warning_message, 
	       "end_group(%d) called with empty group stack\n",
	       group_id);

      WARNING_MESSAGE("Memory::end_group",warning_message);
      
    }

  } else { // curr_group_ out of range

    sprintf (warning_message, "Group %d is out of range [1,%d]\n",
	     group_id, MEMORY_MAX_NUM_GROUPS);

    WARNING_MESSAGE("Memory::end_group",warning_message);
  }
#endif
}

//----------------------------------------------------------------------

const char * Memory::current_group () throw ()
{
#ifdef CONFIG_USE_MEMORY

  // Create "null" group name if needed

  memory_group_handle current = curr_group_.top();

  if (group_names_[current] == 0) {
    group_names_[current] = strdup("");
  }

  return group_names_[current];
#else
  return "";
#endif
}

//----------------------------------------------------------------------

memory_group_handle Memory::current_handle () throw ()
{
#ifdef CONFIG_USE_MEMORY
  return curr_group_.top();
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

long long Memory::bytes ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  return bytes_[group_handle];
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

long long Memory::available ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  INCOMPLETE_MESSAGE("Memory::available()","");
  return 0;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

float Memory::efficiency ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  INCOMPLETE_MESSAGE("Memory::efficiency()","");
  return 0.0;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

long long Memory::highest ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  INCOMPLETE_MESSAGE("Memory::highest(group)","");
  return 0;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

void Memory::set_limit ( long long size, memory_group_handle group_handle )
  throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  available_[group_handle] = size;
#endif
}

//----------------------------------------------------------------------

int Memory::num_new ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  INCOMPLETE_MESSAGE("Memory::num_new()","");
  return 0;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

int Memory::num_delete ( memory_group_handle group_handle ) throw ()
{
#ifdef CONFIG_USE_MEMORY
  check_handle_(group_handle);
  INCOMPLETE_MESSAGE("Memory::num_delete()","");
  return 0;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------

void Memory::print () throw ()
{
#ifdef CONFIG_USE_MEMORY
  for (memory_group_handle i=0; i<= MEMORY_MAX_NUM_GROUPS; i++) {
    if (i == 0 || group_names_[i] != NULL) {
      printf ("Group %s\n",i ? group_names_[i]: "Total");
      if (available_[i]) printf ("   available_    = %ld\n",long(available_[i]));
      printf ("   bytes_        = %ld\n",long(bytes_[i]));
      printf ("   bytes_high_   = %ld\n",long(bytes_high_[i]));
      printf ("   new_calls_    = %ld\n",long(new_calls_[i]));
      printf ("   delete_calls_ = %ld\n",long(delete_calls_[i]));
    }
  }
#endif
}

//----------------------------------------------------------------------

void Memory::reset() throw()
{
#ifdef CONFIG_USE_MEMORY
  while (! curr_group_.empty()) curr_group_.pop();

  curr_group_.push(0);

  for (int i=0; i<MEMORY_MAX_NUM_GROUPS + 1; i++) {
    bytes_       [i] = 0;
    bytes_high_  [i] = 0;
    new_calls_   [i] = 0;
    delete_calls_[i] = 0;
  }
#endif
}

//======================================================================

#ifdef CONFIG_USE_MEMORY
void *operator new (size_t bytes) throw (std::bad_alloc)
{
  size_t p = (size_t) Memory::get_instance()->allocate(bytes);

  // Return pointer to new storage

  return (void *) p;
}

//----------------------------------------------------------------------

void *operator new [] (size_t bytes) throw (std::bad_alloc)
{
  size_t p = (size_t) Memory::get_instance()->allocate(bytes);

  // Return pointer to new storage

  return (void *)(p);

}

//----------------------------------------------------------------------

void operator delete (void *p) throw ()
{
  if (p==0) return;

  Memory::get_instance()->deallocate(p);

}

//----------------------------------------------------------------------

void operator delete [] (void *p) throw ()
{
  if (p==0) return;

  Memory::get_instance()->deallocate(p);
}
#endif /* CONFIG_USE_MEMORY */
